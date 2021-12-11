#include "AlsCharacter.h"

#include "AlsCharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/Core/PushModel/PushModel.h"
#include "RootMotionSource/AlsRootMotionSource_Mantling.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"
#include "Utility/GameplayTags/AlsLocomotionActionTags.h"

bool AAlsCharacter::TryStartMantlingGrounded()
{
	return LocomotionMode == EAlsLocomotionMode::Grounded &&
	       TryStartMantling(GeneralMantlingSettings.GroundedTrace);
}

bool AAlsCharacter::TryStartMantlingInAir()
{
	return LocomotionMode == EAlsLocomotionMode::InAir && IsLocallyControlled() &&
	       TryStartMantling(GeneralMantlingSettings.InAirTrace);
}

bool AAlsCharacter::IsMantlingAllowedToStart() const
{
	return !LocomotionAction.IsValid();
}

bool AAlsCharacter::TryStartMantling(const FAlsMantlingTraceSettings& TraceSettings)
{
	if (!GeneralMantlingSettings.bAllowMantling || GetLocalRole() <= ROLE_SimulatedProxy || !IsMantlingAllowedToStart())
	{
		return false;
	}

	auto ActorLocation{GetActorLocation()};
	auto ActorRotation{GetActorRotation()};

	float ForwardTraceAngle;
	if (LocomotionState.bHasSpeed)
	{
		ForwardTraceAngle = LocomotionState.bHasInput
			                    ? LocomotionState.VelocityYawAngle +
			                      FMath::ClampAngle(LocomotionState.InputYawAngle - LocomotionState.VelocityYawAngle,
			                                        -GeneralMantlingSettings.MaxReachAngle, GeneralMantlingSettings.MaxReachAngle)
			                    : LocomotionState.VelocityYawAngle;
	}
	else
	{
		ForwardTraceAngle = LocomotionState.bHasInput
			                    ? LocomotionState.InputYawAngle
			                    : ActorRotation.Yaw;
	}

	const auto ForwardTraceDeltaAngle{ForwardTraceAngle - ActorRotation.Yaw};
	if (FMath::Abs(ForwardTraceDeltaAngle) > GeneralMantlingSettings.TraceAngleThreshold)
	{
		return false;
	}

	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : GeneralMantlingSettings.MantlingTraceObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}

	const auto ForwardTraceDirection{
		UAlsMath::AngleToDirection2D(
			ActorRotation.Yaw +
			FMath::ClampAngle(ForwardTraceDeltaAngle, -GeneralMantlingSettings.MaxReachAngle, GeneralMantlingSettings.MaxReachAngle))
	};

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebug(this, UAlsConstants::MantlingDisplayName())};
#endif

	const auto* Capsule{GetCapsuleComponent()};

	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	const auto CapsuleBottomLocation{
		ActorLocation - GetActorQuat().RotateVector(FVector::UpVector) * CapsuleHalfHeight
	};

	const auto TraceCapsuleRadius{CapsuleRadius - 1.0f};

	const auto LedgeHeightDelta{TraceSettings.LedgeHeight.GetMax() - TraceSettings.LedgeHeight.GetMin()};

	// Trace forward to find a object the character cannot walk on.

	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	auto ForwardTraceStart{CapsuleBottomLocation - ForwardTraceDirection * CapsuleRadius};
	ForwardTraceStart.Z += (TraceSettings.LedgeHeight.X + TraceSettings.LedgeHeight.Y) * 0.5f - UCharacterMovementComponent::MAX_FLOOR_DIST;

	auto ForwardTraceEnd{ForwardTraceStart + ForwardTraceDirection * (CapsuleRadius + TraceSettings.ReachDistance + 1.0f)};

	const auto ForwardTraceCapsuleHalfHeight{LedgeHeightDelta * 0.5f};

	FHitResult ForwardTraceHit;
	GetWorld()->SweepSingleByObjectType(ForwardTraceHit, ForwardTraceStart, ForwardTraceEnd, FQuat::Identity, ObjectQueryParameters,
	                                    FCollisionShape::MakeCapsule(TraceCapsuleRadius, ForwardTraceCapsuleHalfHeight),
	                                    {ForwardTraceTag, false, this});

	auto* TargetPrimitive{ForwardTraceHit.GetComponent()};

	if (!ForwardTraceHit.IsValidBlockingHit() ||
	    !IsValid(TargetPrimitive) ||
	    TargetPrimitive->GetComponentVelocity().SizeSquared() > FMath::Square(GeneralMantlingSettings.TargetPrimitiveSpeedThreshold) ||
	    !TargetPrimitive->CanCharacterStepUp(this) ||
	    GetCharacterMovement()->IsWalkable(ForwardTraceHit))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, false, ForwardTraceHit, {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);
		}
#endif

		return false;
	}

	// Trace downward from the first trace's impact point and determine if the hit location is walkable.

	static const FName DownwardTraceTag{FString::Format(TEXT("{0} (Downward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	const auto TargetLocationOffset{FVector2D{ForwardTraceHit.ImpactNormal.GetSafeNormal2D()} * TraceSettings.TargetLocationOffset};

	const FVector DownwardTraceStart{
		ForwardTraceHit.ImpactPoint.X - TargetLocationOffset.X, ForwardTraceHit.ImpactPoint.Y - TargetLocationOffset.Y,
		CapsuleBottomLocation.Z + LedgeHeightDelta + 2.5f * TraceCapsuleRadius + UCharacterMovementComponent::MIN_FLOOR_DIST
	};

	const FVector DownwardTraceEnd{
		DownwardTraceStart.X, DownwardTraceStart.Y,
		CapsuleBottomLocation.Z + TraceSettings.LedgeHeight.GetMin() + TraceCapsuleRadius - UCharacterMovementComponent::MAX_FLOOR_DIST
	};

	FHitResult DownwardTraceHit;
	GetWorld()->SweepSingleByObjectType(DownwardTraceHit, DownwardTraceStart, DownwardTraceEnd, FQuat::Identity,
	                                    ObjectQueryParameters, FCollisionShape::MakeSphere(TraceCapsuleRadius),
	                                    {DownwardTraceTag, false, this});

	if (!GetCharacterMovement()->IsWalkable(DownwardTraceHit))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit, {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);

			UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd, TraceCapsuleRadius,
			                                        false, DownwardTraceHit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f},
			                                        TraceSettings.bDrawFailedTraces ? 7.5f : 0.0f);
		}
#endif

		return false;
	}

	// Check if the capsule has room to stand at the downward trace's location. If so,
	// set that location as the target transform and calculate the mantling height.

	static const FName FreeSpaceTraceTag{FString::Format(TEXT("{0} (Free Space Overlap)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	const FVector TargetLocation{
		DownwardTraceHit.ImpactPoint.X, DownwardTraceHit.ImpactPoint.Y,
		DownwardTraceHit.ImpactPoint.Z + CapsuleHalfHeight + UCharacterMovementComponent::MIN_FLOOR_DIST
	};

	if (GetWorld()->OverlapAnyTestByObjectType(TargetLocation, FQuat::Identity, ObjectQueryParameters,
	                                           FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
	                                           {FreeSpaceTraceTag, false, this}))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit, {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);

			UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd, TraceCapsuleRadius,
			                                        false, DownwardTraceHit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f},
			                                        TraceSettings.bDrawFailedTraces ? 7.5f : 0.0f);

			DrawDebugCapsule(Capsule->GetWorld(), TargetLocation, CapsuleHalfHeight, CapsuleRadius,
			                 FQuat::Identity, FColor::Red, false, TraceSettings.bDrawFailedTraces ? 10.0f : 0.0f);
		}
#endif

		return false;
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebug)
	{
		UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
		                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit,
		                                                    {0.0f, 0.25f, 1.0f}, {0.0f, 0.75f, 1.0f}, 5.0f);

		UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd,
		                                        TraceCapsuleRadius, true, DownwardTraceHit,
		                                        {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f}, 7.5f);
	}
#endif

	const auto TargetRotation{(-ForwardTraceHit.ImpactNormal.GetSafeNormal2D()).ToOrientationQuat()};

	FAlsMantlingParameters Parameters;

	Parameters.TargetPrimitive = TargetPrimitive;
	Parameters.MantlingHeight = TargetLocation.Z - ActorLocation.Z;

	// Determine the mantling type by checking the movement mode and mantling height.

	Parameters.MantlingType = LocomotionMode == EAlsLocomotionMode::Grounded
		                          ? Parameters.MantlingHeight > 125.0f
			                            ? EAlsMantlingType::High
			                            : EAlsMantlingType::Low
		                          : EAlsMantlingType::InAir;

	// If the target primitive can't move, then use world coordinates in order to
	// save some performance by skipping some coordinate space transformations later.

	if (MovementBaseUtility::UseRelativeLocation(TargetPrimitive))
	{
		const auto TargetRelativeTransform{
			TargetPrimitive->GetComponentTransform().GetRelativeTransform({TargetRotation, TargetLocation})
		};

		Parameters.TargetRelativeLocation = TargetRelativeTransform.GetLocation();
		Parameters.TargetRelativeRotation = TargetRelativeTransform.Rotator();
	}
	else
	{
		Parameters.TargetRelativeLocation = TargetLocation;
		Parameters.TargetRelativeRotation = TargetRotation.Rotator();
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartMantling(Parameters);
	}
	else
	{
		GetCharacterMovement()->FlushServerMoves();

		StartMantlingImplementation(Parameters);
		ServerStartMantling(Parameters);
	}

	return true;
}

void AAlsCharacter::ServerStartMantling_Implementation(const FAlsMantlingParameters& Parameters)
{
	if (IsMantlingAllowedToStart())
	{
		MulticastStartMantling(Parameters);
		ForceNetUpdate();
	}
}

void AAlsCharacter::MulticastStartMantling_Implementation(const FAlsMantlingParameters& Parameters)
{
	StartMantlingImplementation(Parameters);
}

void AAlsCharacter::StartMantlingImplementation(const FAlsMantlingParameters& Parameters)
{
	if (!IsMantlingAllowedToStart())
	{
		return;
	}

	auto* MantlingSettings{SelectMantlingSettings(Parameters.MantlingType)};
	if (!IsValid(MantlingSettings) || !IsValid(MantlingSettings->BlendInCurve) ||
	    !IsValid(MantlingSettings->InterpolationAndCorrectionAmountsCurve))
	{
		return;
	}

	const auto StartTime{MantlingSettings->CalculateStartTime(Parameters.MantlingHeight)};
	const auto PlayRate{MantlingSettings->CalculatePlayRate(Parameters.MantlingHeight)};

	// Calculate mantling duration.

	auto MinTime{0.0f};
	auto MaxTime{0.0f};
	MantlingSettings->InterpolationAndCorrectionAmountsCurve->GetTimeRange(MinTime, MaxTime);

	const auto Duration{MaxTime - StartTime};

	// Calculate actor offsets (offsets between actor and target transform).

	const auto bUseRelativeLocation{MovementBaseUtility::UseRelativeLocation(Parameters.TargetPrimitive.Get())};

	const auto TargetTransform{
		bUseRelativeLocation
			? FTransform{
				Parameters.TargetRelativeRotation, Parameters.TargetRelativeLocation,
				Parameters.TargetPrimitive->GetComponentScale()
			}.GetRelativeTransformReverse(Parameters.TargetPrimitive->GetComponentTransform())
			: FTransform{Parameters.TargetRelativeRotation, Parameters.TargetRelativeLocation}
	};

	const auto ActorLocationOffset{GetActorLocation() - TargetTransform.GetLocation()};
	const auto ActorRotationOffset{TargetTransform.GetRotation().Inverse() * GetActorQuat()};

	// Clear the character movement mode and set the locomotion action to mantling.

	GetCharacterMovement()->SetMovementMode(MOVE_Custom);
	GetCharacterMovement()->SetBase(Parameters.TargetPrimitive.Get());
	AlsCharacterMovement->SetMovementModeLocked(true);

	if (GetLocalRole() >= ROLE_Authority)
	{
		GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
		GetMesh()->SetRelativeLocationAndRotation(BaseTranslationOffset, BaseRotationOffset);
	}

	// Apply mantling root motion.

	const auto Mantling{MakeShared<FAlsRootMotionSource_Mantling>()};
	Mantling->InstanceName = ANSI_TO_TCHAR(__FUNCTION__);
	Mantling->Duration = Duration / PlayRate;
	Mantling->MantlingSettings = MantlingSettings;
	Mantling->TargetPrimitive = bUseRelativeLocation ? Parameters.TargetPrimitive : nullptr;
	Mantling->TargetRelativeLocation = Parameters.TargetRelativeLocation;
	Mantling->TargetRelativeRotation = Parameters.TargetRelativeRotation;
	Mantling->ActorLocationOffset = ActorLocationOffset;
	Mantling->ActorRotationOffset = ActorRotationOffset.Rotator();
	Mantling->MantlingHeight = Parameters.MantlingHeight;

	MantlingRootMotionSourceId = GetCharacterMovement()->ApplyRootMotionSource(Mantling);

	// Play the animation montage if valid.

	if (IsValid(MantlingSettings->Montage))
	{
		// Magic. I can't explain why, but this code fixes animation and root motion source desynchronization.

		const auto MontageStartTime{
			Parameters.MantlingType == EAlsMantlingType::InAir && IsLocallyControlled()
				? StartTime - FMath::GetMappedRangeValueClamped(
					  MantlingSettings->ReferenceHeight, {GetWorld()->GetDeltaSeconds(), 0.0f}, Parameters.MantlingHeight)
				: StartTime
		};

		if (GetMesh()->GetAnimInstance()->Montage_Play(MantlingSettings->Montage, PlayRate,
		                                               EMontagePlayReturnType::MontageLength,
		                                               MontageStartTime, false))
		{
			SetLocomotionAction(FAlsLocomotionActionTags::Get().Mantling);
		}
	}

	OnMantlingStarted(Parameters);
}

UAlsMantlingSettings* AAlsCharacter::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return nullptr;
}

void AAlsCharacter::OnMantlingStarted_Implementation(const FAlsMantlingParameters& Parameters) {}

void AAlsCharacter::RefreshMantling()
{
	if (MantlingRootMotionSourceId <= 0)
	{
		return;
	}

	const auto RootMotionSource{GetCharacterMovement()->GetRootMotionSourceByID(MantlingRootMotionSourceId)};
	if (RootMotionSource.IsValid() &&
	    !RootMotionSource->Status.HasFlag(ERootMotionSourceStatusFlags::Finished) &&
	    !RootMotionSource->Status.HasFlag(ERootMotionSourceStatusFlags::MarkedForRemoval))
	{
		RefreshTargetYawAngle(LocomotionState.Rotation.Yaw);
	}
	else
	{
		StopMantling();
		ForceNetUpdate();
	}
}

void AAlsCharacter::StopMantling()
{
	if (MantlingRootMotionSourceId <= 0)
	{
		return;
	}

	const auto RootMotionSource{GetCharacterMovement()->GetRootMotionSourceByID(MantlingRootMotionSourceId)};
	if (RootMotionSource.IsValid() &&
	    !RootMotionSource->Status.HasFlag(ERootMotionSourceStatusFlags::Finished) &&
	    !RootMotionSource->Status.HasFlag(ERootMotionSourceStatusFlags::MarkedForRemoval))
	{
		RootMotionSource->Status.SetFlag(ERootMotionSourceStatusFlags::MarkedForRemoval);
	}

	MantlingRootMotionSourceId = 0;

	if (GetLocalRole() >= ROLE_Authority)
	{
		GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Exponential;
	}

	AlsCharacterMovement->SetMovementModeLocked(false);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	OnMantlingEnded();
}

void AAlsCharacter::OnMantlingEnded_Implementation() {}

bool AAlsCharacter::IsRagdollingAllowedToStart() const
{
	return LocomotionAction != FAlsLocomotionActionTags::Get().Ragdolling;
}

void AAlsCharacter::StartRagdolling()
{
	if (GetLocalRole() <= ROLE_SimulatedProxy || !IsRagdollingAllowedToStart())
	{
		return;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartRagdolling();
	}
	else
	{
		GetCharacterMovement()->FlushServerMoves();

		ServerStartRagdolling();
	}
}

void AAlsCharacter::ServerStartRagdolling_Implementation()
{
	if (IsRagdollingAllowedToStart())
	{
		MulticastStartRagdolling();
		ForceNetUpdate();
	}
}

void AAlsCharacter::MulticastStartRagdolling_Implementation()
{
	StartRagdollingImplementation();
}

void AAlsCharacter::StartRagdollingImplementation()
{
	if (!IsRagdollingAllowedToStart())
	{
		return;
	}

	// Stop any active montages.

	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);

	// When networked, disable replicate movement reset ragdolling target location and pull force variables.

	SetReplicateMovement(false);
	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;

	// Clear the character movement mode and set the locomotion action to ragdolling.

	GetCharacterMovement()->SetMovementMode(MOVE_None);
	AlsCharacterMovement->SetMovementModeLocked(true);

	SetLocomotionAction(FAlsLocomotionActionTags::Get().Ragdolling);

	// Disable capsule collision and enable mesh physics simulation starting from the pelvis.

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(UAlsConstants::PelvisBone(), true, true);

	RagdollingState.PullForce = 0.0f;

	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		SetRagdollTargetLocation(GetMesh()->GetSocketLocation(UAlsConstants::PelvisBone()));
	}

	OnRagdollingStarted();
}

void AAlsCharacter::OnRagdollingStarted_Implementation() {}

void AAlsCharacter::SetRagdollTargetLocation(const FVector& NewLocation)
{
	if (RagdollTargetLocation != NewLocation)
	{
		RagdollTargetLocation = NewLocation;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RagdollTargetLocation, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetRagdollTargetLocation(NewLocation);
		}
	}
}

void AAlsCharacter::ServerSetRagdollTargetLocation_Implementation(const FVector& NewTargetLocation)
{
	SetRagdollTargetLocation(NewTargetLocation);
}

void AAlsCharacter::RefreshRagdolling(const float DeltaTime)
{
	if (LocomotionAction != FAlsLocomotionActionTags::Get().Ragdolling)
	{
		return;
	}

	if (IsNetMode(NM_DedicatedServer))
	{
		// Change animation tick option when the host is a dedicated server to avoid z-location bug.
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}

	const auto RootBoneVelocity{GetMesh()->GetPhysicsLinearVelocity(UAlsConstants::RootBone())};

	RagdollingState.RootBoneVelocity = RootBoneVelocity.SizeSquared() > SMALL_NUMBER || IsLocallyControlled()
		                                   ? RootBoneVelocity
		                                   : RagdollingState.RootBoneVelocity * 0.5f;

	// Use the velocity to scale the ragdoll's joint strength for physical animation.

	GetMesh()->SetAllMotorsAngularDriveParams(UAlsMath::Clamp01(RagdollingState.RootBoneVelocity.Size() / 1000.0f) * 25000.0f,
	                                          0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual
	// acceleration. This also prevents the ragdoll from going through the floor.

	GetMesh()->SetEnableGravity(RagdollingState.RootBoneVelocity.Z > -4000.0f);

	RefreshRagdollingActorTransform(DeltaTime);
}

void AAlsCharacter::RefreshRagdollingActorTransform(const float DeltaTime)
{
	const auto bLocallyControlled{IsLocallyControlled()};

	const auto PelvisTransform{GetMesh()->GetSocketTransform(UAlsConstants::PelvisBone())};

	if (bLocallyControlled)
	{
		SetRagdollTargetLocation(PelvisTransform.GetLocation());
	}

	// Trace downward from the target location to offset the target location, preventing the lower
	// half of the capsule from going through the floor when the ragdoll is laying on the ground.

	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : RagdollingSettings.GroundTraceObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, RagdollTargetLocation, {
		                                        RagdollTargetLocation.X,
		                                        RagdollTargetLocation.Y,
		                                        RagdollTargetLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
	                                        }, ObjectQueryParameters, {ANSI_TO_TCHAR(__FUNCTION__), false, this});

	auto NewActorLocation{RagdollTargetLocation};

	RagdollingState.bGrounded = Hit.IsValidBlockingHit();

	if (RagdollingState.bGrounded)
	{
		NewActorLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - FMath::Abs(Hit.ImpactPoint.Z - Hit.TraceStart.Z) + 2.0f;
	}

	if (!bLocallyControlled)
	{
		RagdollingState.PullForce = FMath::FInterpTo(RagdollingState.PullForce, 750.0f, DeltaTime, 0.6f);

		const auto RagdollSpeedSquared{FVector2D{RagdollingState.RootBoneVelocity.X, RagdollingState.RootBoneVelocity.Y}.SizeSquared()};

		const auto PullForceSocketName{
			RagdollSpeedSquared > FMath::Square(300.0f)
				? UAlsConstants::Spine03Bone()
				: UAlsConstants::PelvisBone()
		};

		GetMesh()->AddForce((RagdollTargetLocation - GetMesh()->GetSocketLocation(PullForceSocketName)) * RagdollingState.PullForce,
		                    PullForceSocketName, true);
	}

	// Determine whether the ragdoll is facing upward or downward and set the target rotation accordingly.

	const auto PelvisRotation{PelvisTransform.Rotator()};

	RagdollingState.bFacedUpward = PelvisRotation.Roll < 0.0f;

	SetActorLocationAndRotation(NewActorLocation, {
		                            0.0f, RagdollingState.bFacedUpward ? PelvisRotation.Yaw - 180.0f : PelvisRotation.Yaw, 0.0f
	                            });

	RefreshLocomotionLocationAndRotation();

	RefreshTargetYawAngle(LocomotionState.Rotation.Yaw);
}

bool AAlsCharacter::IsRagdollingAllowedToStop() const
{
	return LocomotionAction == FAlsLocomotionActionTags::Get().Ragdolling;
}

bool AAlsCharacter::TryStopRagdolling()
{
	if (GetLocalRole() <= ROLE_SimulatedProxy || !IsRagdollingAllowedToStop())
	{
		return false;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStopRagdolling();
	}
	else
	{
		ServerStopRagdolling();
	}

	return true;
}

void AAlsCharacter::ServerStopRagdolling_Implementation()
{
	if (IsRagdollingAllowedToStop())
	{
		MulticastStopRagdolling();
		ForceNetUpdate();
	}
}

void AAlsCharacter::MulticastStopRagdolling_Implementation()
{
	StopRagdollingImplementation();
}

void AAlsCharacter::StopRagdollingImplementation()
{
	if (!IsRagdollingAllowedToStop())
	{
		return;
	}

	// Re-enable capsule collision and disable physics simulation on the mesh.

	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SetLocomotionAction(FGameplayTag::EmptyTag);

	// If the ragdoll is on the ground, set the movement mode to walking and play a get up animation. If not, set
	// the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.

	AlsCharacterMovement->SetMovementModeLocked(false);

	if (RagdollingState.bGrounded)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = RagdollingState.RootBoneVelocity;
	}

	// Re-enable replicate movement.

	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;
	SetReplicateMovement(true);

	OnRagdollingEnded();

	if (RagdollingState.bGrounded &&
	    GetMesh()->GetAnimInstance()->Montage_Play(SelectGetUpMontage(RagdollingState.bFacedUpward), 1.0f,
	                                               EMontagePlayReturnType::MontageLength, 0.0f, true))
	{
		SetLocomotionAction(FAlsLocomotionActionTags::Get().GettingUp);
	}
}

UAnimMontage* AAlsCharacter::SelectGetUpMontage_Implementation(bool bRagdollFacedUpward)
{
	return nullptr;
}

void AAlsCharacter::OnRagdollingEnded_Implementation() {}

void AAlsCharacter::TryStartRolling(const float PlayRate)
{
	if (LocomotionMode != EAlsLocomotionMode::Grounded)
	{
		return;
	}

	StartRolling(PlayRate, LocomotionState.bHasInput &&
	                       (RollingSettings.bRotateToInputOnStart ||
	                        RollingSettings.bRotateToInputDuringRoll && RollingSettings.InputInterpolationSpeed <= 0.0f)
		                       ? LocomotionState.InputYawAngle
		                       : GetActorRotation().Yaw);
}

bool AAlsCharacter::IsRollingAllowedToStart(const UAnimMontage* Montage) const
{
	return !LocomotionAction.IsValid() ||
	       // ReSharper disable once CppRedundantParentheses
	       (LocomotionAction == FAlsLocomotionActionTags::Get().Rolling &&
	        !GetMesh()->GetAnimInstance()->Montage_IsPlaying(Montage));
}

void AAlsCharacter::StartRolling(const float PlayRate, const float TargetYawAngle)
{
	if (GetLocalRole() <= ROLE_SimulatedProxy)
	{
		return;
	}

	auto* Montage{SelectRollMontage()};
	if (!IsValid(Montage) || !IsRollingAllowedToStart(Montage))
	{
		return;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartRolling(Montage, PlayRate, TargetYawAngle);
	}
	else
	{
		GetCharacterMovement()->FlushServerMoves();

		StartRollingImplementation(Montage, PlayRate, TargetYawAngle);
		ServerStartRolling(Montage, PlayRate, TargetYawAngle);
	}
}

UAnimMontage* AAlsCharacter::SelectRollMontage_Implementation()
{
	return nullptr;
}

void AAlsCharacter::ServerStartRolling_Implementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	if (IsRollingAllowedToStart(Montage))
	{
		MulticastStartRolling(Montage, PlayRate, TargetYawAngle);
		ForceNetUpdate();
	}
}

void AAlsCharacter::MulticastStartRolling_Implementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	StartRollingImplementation(Montage, PlayRate, TargetYawAngle);
}

void AAlsCharacter::StartRollingImplementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	if (!IsRollingAllowedToStart(Montage))
	{
		return;
	}

	if (RollingSettings.ActorRotationInterpolationSpeed <= 0.0f)
	{
		RefreshActorRotationInstant(TargetYawAngle, ETeleportType::TeleportPhysics);
	}
	else
	{
		RefreshTargetYawAngle(TargetYawAngle, false);
	}

	if (GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate))
	{
		SetLocomotionAction(FAlsLocomotionActionTags::Get().Rolling);
	}
}

void AAlsCharacter::RefreshRolling(const float DeltaTime)
{
	if (LocomotionState.bRotationLocked || LocomotionAction != FAlsLocomotionActionTags::Get().Rolling)
	{
		return;
	}

	if (RollingSettings.bRotateToInputDuringRoll && LocomotionState.bHasInput)
	{
		RefreshTargetYawAngle(UAlsMath::InterpolateAngleConstant(LocomotionState.TargetYawAngle,
		                                                         LocomotionState.InputYawAngle,
		                                                         DeltaTime, RollingSettings.InputInterpolationSpeed));
	}

	RefreshActorRotation(LocomotionState.TargetYawAngle, DeltaTime, RollingSettings.ActorRotationInterpolationSpeed);
}
