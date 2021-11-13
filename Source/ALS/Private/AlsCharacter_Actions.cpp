#include "AlsCharacter.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

bool AAlsCharacter::TryStartMantlingGrounded()
{
	return LocomotionMode == EAlsLocomotionMode::Grounded &&
	       TryStartMantling(GeneralMantlingSettings.GroundedTrace);
}

bool AAlsCharacter::TryStartMantlingInAir()
{
	return TryStartMantling(GeneralMantlingSettings.InAirTrace);
}

bool AAlsCharacter::IsMantlingAllowedToStart() const
{
	// TODO Compare the parameters of the currently running mantling and
	// the new one to decide whether to replace it with a new one or not.

	return (LocomotionMode == EAlsLocomotionMode::Grounded ||
	        LocomotionMode == EAlsLocomotionMode::InAir) &&
	       LocomotionAction == EAlsLocomotionAction::None;
}

bool AAlsCharacter::TryStartMantling(const FAlsMantlingTraceSettings& TraceSettings)
{
	if (!GeneralMantlingSettings.bAllowMantling || !IsMantlingAllowedToStart() || GetLocalRole() <= ROLE_SimulatedProxy)
	{
		return false;
	}

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
			                    : LocomotionState.Rotation.Yaw;
	}

	const auto ForwardTraceDeltaAngle{ForwardTraceAngle - LocomotionState.Rotation.Yaw};
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
			LocomotionState.Rotation.Yaw +
			FMath::ClampAngle(ForwardTraceDeltaAngle, -GeneralMantlingSettings.MaxReachAngle, GeneralMantlingSettings.MaxReachAngle))
	};

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebug(this, UAlsConstants::MantlingDisplayName())};
#endif

	const auto* Capsule{GetCapsuleComponent()};

	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	const auto CapsuleBottomLocation{
		LocomotionState.Location - LocomotionState.Rotation.RotateVector(FVector::UpVector) * CapsuleHalfHeight
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
	    !ForwardTraceHit.Component->CanCharacterStepUp(this) ||
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

	const auto TargetRotation{(-ForwardTraceHit.ImpactNormal.GetSafeNormal2D()).ToOrientationRotator()};

	const auto MantlingHeight{TargetLocation.Z - LocomotionState.Location.Z};

	// Determine the mantling type by checking the movement mode and mantling height.

	EAlsMantlingType MantlingType;
	if (LocomotionMode == EAlsLocomotionMode::InAir)
	{
		MantlingType = EAlsMantlingType::InAir;
	}
	else
	{
		MantlingType = MantlingHeight > 125.0f
			               ? EAlsMantlingType::High
			               : EAlsMantlingType::Low;
	}

	const FAlsMantlingParameters Parameters{
		TargetPrimitive,
		TargetPrimitive->GetComponentTransform().GetRelativeTransform({TargetRotation, TargetLocation}),
		MantlingHeight, MantlingType
	};

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartMantling(Parameters);
	}
	else
	{
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
	if (!IsLocallyControlled() || IsMantlingAllowedToStart())
	{
		StartMantlingImplementation(Parameters);
	}
}

void AAlsCharacter::StartMantlingImplementation(const FAlsMantlingParameters& Parameters)
{
	if (LocomotionMode != EAlsLocomotionMode::Mantling)
	{
		// This will help to get rid of the jitter on the client side due to mispredictions of the character's future position.

		MantlingState.PreviousNetworkSmoothingMode = GetCharacterMovement()->NetworkSmoothingMode;

		GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
	}

	GetMesh()->SetRelativeLocationAndRotation(BaseTranslationOffset, BaseRotationOffset);

	LocomotionState.Location = GetActorLocation();
	LocomotionState.Rotation = GetActorRotation();

	// Selects the mantling settings and use it to set the new mantling state.

	const auto MantlingSettings{SelectMantlingSettings(Parameters.MantlingType)};

	MantlingState.Montage = MantlingSettings.Montage;
	MantlingState.InterpolationAndCorrectionCurve = MantlingSettings.InterpolationAndCorrectionAmountsCurve;

	MantlingState.WorldStartTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		MantlingState.WorldStartTime -= GetWorld()->GetDeltaSeconds(); // Magic.
	}

	MantlingState.StartTime = FMath::GetMappedRangeValueClamped(MantlingSettings.ReferenceHeight, MantlingSettings.StartTime,
	                                                            Parameters.MantlingHeight);

	MantlingState.PlayRate = FMath::GetMappedRangeValueClamped(MantlingSettings.ReferenceHeight, MantlingSettings.PlayRate,
	                                                           Parameters.MantlingHeight);

	// Configure the mantling duration.

	auto MinTime{0.0f};
	auto MaxTime{0.0f};
	MantlingState.InterpolationAndCorrectionCurve->GetTimeRange(MinTime, MaxTime);

	MantlingState.Duration = MaxTime - MantlingState.StartTime;

	MantlingState.Primitive = Parameters.Primitive;
	MantlingState.RelativeTransform = Parameters.RelativeTransform;

	// Convert the primitive's local space transform to the world space space for use in moving objects.

	const auto TargetTransform{
		MantlingState.RelativeTransform.GetRelativeTransformReverse(MantlingState.Primitive->GetComponentTransform())
	};

	// Calculate the actor offset transform (offset amount between the actor and target transform).

	MantlingState.ActorOffset = UAlsMath::SubtractTransforms(CalculateNetworkSmoothedTransform(), TargetTransform);

	// Calculate the animation offset transform from the target location. This would be
	// the location the actual animation starts at relative to the target transform.

	auto AnimationOffsetLocation{TargetTransform.GetRotation().Vector() * MantlingSettings.StartRelativeLocation.X};
	AnimationOffsetLocation.Z = MantlingSettings.StartRelativeLocation.Z;

	MantlingState.AnimationOffset = AnimationOffsetLocation;

	// Clear the character movement mode and set the movement state to mantling.

	if (LocomotionMode != EAlsLocomotionMode::Mantling)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		GetCharacterMovement()->SetBase(MantlingState.Primitive);

		SetLocomotionMode(EAlsLocomotionMode::Mantling);
	}

	// Play the animation montage if valid.

	if (IsValid(MantlingState.Montage))
	{
		GetMesh()->GetAnimInstance()->Montage_Play(MantlingState.Montage, MantlingState.PlayRate,
		                                           EMontagePlayReturnType::MontageLength, MantlingState.StartTime, false);
	}

	OnMantlingStarted(Parameters);
}

FAlsMantlingSettings AAlsCharacter::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return {};
}

void AAlsCharacter::OnMantlingStarted_Implementation(const FAlsMantlingParameters& Parameters) {}

void AAlsCharacter::RefreshMantling()
{
	auto Time{(GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - MantlingState.WorldStartTime) * MantlingState.PlayRate};

	auto bStopMantling{false};
	if (Time >= MantlingState.Duration)
	{
		bStopMantling = true;
		Time = MantlingState.Duration;
	}

	// Update the interpolation and correction amounts using the interpolation and correction amounts curve.

	const auto InterpolationAndCorrectionAmounts{
		MantlingState.InterpolationAndCorrectionCurve->GetVectorValue(MantlingState.StartTime + Time)
	};

	// Lerp multiple transforms together for independent control over the horizontal
	// and vertical blend to the animation offset, as well as the target transform.

	// Blend into the animation horizontal offset.

	auto ResultHorizontalOffset{MantlingState.ActorOffset};
	ResultHorizontalOffset.BlendWith(FTransform{MantlingState.AnimationOffset}, InterpolationAndCorrectionAmounts.Y);

	// Blend into the animation vertical offset.

	const auto ResultVerticalOffset{
		FMath::Lerp(MantlingState.ActorOffset.GetLocation().Z, MantlingState.AnimationOffset.Z, InterpolationAndCorrectionAmounts.Z)
	};

	// Continually update the target transform from the stored relative transform to follow along with moving objects.

	const auto TargetTransform{
		MantlingState.RelativeTransform.GetRelativeTransformReverse(MantlingState.Primitive->GetComponentTransform())
	};

	// Blend from the current blending transforms into the final transform.

	const auto InterpolationAmount{1.0f - UAlsMath::Clamp01(InterpolationAndCorrectionAmounts.X)};

	const FTransform OffsetTransform{
		TargetTransform.Rotator() + ResultHorizontalOffset.Rotator() * InterpolationAmount,
		TargetTransform.GetLocation() + FVector{
			ResultHorizontalOffset.GetLocation().X,
			ResultHorizontalOffset.GetLocation().Y,
			ResultVerticalOffset
		} * InterpolationAmount
	};

	// Initial blend in to allow the actor to blend into the interpolation and correction curve
	// at the midpoint. This prevents pops when mantling an object lower than the animated mantling.

	auto ResultTransform{UAlsMath::AddTransforms(TargetTransform, MantlingState.ActorOffset)};
	ResultTransform.BlendWith(OffsetTransform, GeneralMantlingSettings.BlendInCurve->GetFloatValue(Time));

	SetActorLocationAndRotation(ResultTransform.GetLocation(), ResultTransform.GetRotation());

	GetCharacterMovement()->SaveBaseLocation();

	GetMesh()->SetRelativeLocationAndRotation(BaseTranslationOffset, BaseRotationOffset);

	RefreshLocomotionLocationAndRotation();

	RefreshTargetYawAngle(LocomotionState.Rotation.Yaw);

	if (bStopMantling)
	{
		// Set the character movement mode to walking.

		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		OnMantlingEnded();
	}
}

void AAlsCharacter::OnMantlingEnded_Implementation() {}

bool AAlsCharacter::IsRagdollingAllowedToStart() const
{
	return LocomotionMode != EAlsLocomotionMode::Ragdolling;
}

void AAlsCharacter::StartRagdolling()
{
	if (!IsRagdollingAllowedToStart() || GetLocalRole() <= ROLE_SimulatedProxy)
	{
		return;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartRagdolling();
	}
	else
	{
		ServerStartRagdolling();
	}
}

void AAlsCharacter::ServerStartRagdolling_Implementation()
{
	if (IsRagdollingAllowedToStart())
	{
		MulticastStartRagdolling();
	}
}

void AAlsCharacter::MulticastStartRagdolling_Implementation()
{
	if (!IsLocallyControlled() || IsRagdollingAllowedToStart())
	{
		StartRagdollingImplementation();
	}
}

void AAlsCharacter::StartRagdollingImplementation()
{
	// Stop any active montages.

	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);

	// Force interrupt any active locomotion action without waiting for the UAlsAnimNotifyState_SetLocomotionAction animation notify to end.

	SetLocomotionAction(EAlsLocomotionAction::None);

	if (LocomotionMode != EAlsLocomotionMode::Ragdolling)
	{
		// When networked, disable replicate movement reset ragdolling target location and pull force variables.

		SetReplicateMovement(false);
		GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;

		// Clear the character movement mode and set the movement state to ragdolling.

		GetCharacterMovement()->SetMovementMode(MOVE_None);
		SetLocomotionMode(EAlsLocomotionMode::Ragdolling);

		// Disable capsule collision and enable mesh physics simulation starting from the pelvis.

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetAllBodiesBelowSimulatePhysics(UAlsConstants::PelvisBone(), true, true);
	}

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
	return LocomotionMode == EAlsLocomotionMode::Ragdolling;
}

bool AAlsCharacter::TryStopRagdolling()
{
	if (!IsRagdollingAllowedToStop() || GetLocalRole() <= ROLE_SimulatedProxy)
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
	if (!IsLocallyControlled() || IsRagdollingAllowedToStop())
	{
		StopRagdollingImplementation();
	}
}

void AAlsCharacter::StopRagdollingImplementation()
{
	if (LocomotionMode == EAlsLocomotionMode::Ragdolling)
	{
		// Re-enable replicate movement.

		SetReplicateMovement(true);
		GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;

		// If the ragdoll is on the ground, set the movement mode to walking and play a get up animation. If not, set
		// the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.

		if (RagdollingState.bGrounded)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		else
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			GetCharacterMovement()->Velocity = RagdollingState.RootBoneVelocity;
		}

		// Re-enable capsule collision and disable physics simulation on the mesh.

		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetCollisionObjectType(ECC_Pawn);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (RagdollingState.bGrounded)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SelectGetUpMontage(RagdollingState.bFacedUpward), 1.0f,
		                                           EMontagePlayReturnType::MontageLength, 0.0f, true);
	}

	OnRagdollingEnded();
}

UAnimMontage* AAlsCharacter::SelectGetUpMontage_Implementation(bool bRagdollFacedUpward)
{
	return nullptr;
}

void AAlsCharacter::OnRagdollingEnded_Implementation() {}

void AAlsCharacter::TryStartRolling(const float PlayRate)
{
	if (LocomotionMode == EAlsLocomotionMode::Grounded)
	{
		StartRolling(PlayRate, LocomotionState.bHasInput &&
		                       (RollingSettings.bRotateToInputOnStart ||
		                        RollingSettings.bRotateToInputDuringRoll && RollingSettings.InputInterpolationSpeed <= 0.0f)
			                       ? LocomotionState.InputYawAngle
			                       : LocomotionState.Rotation.Yaw);
	}
}

bool AAlsCharacter::IsRollingAllowedToStart(const UAnimMontage* Montage) const
{
	return LocomotionAction == EAlsLocomotionAction::None ||
	       // ReSharper disable once CppRedundantParentheses
	       (LocomotionAction == EAlsLocomotionAction::Rolling &&
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
	if (!IsLocallyControlled() || IsRollingAllowedToStart(Montage))
	{
		StartRollingImplementation(Montage, PlayRate, TargetYawAngle);
	}
}

void AAlsCharacter::StartRollingImplementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	if (RollingSettings.ActorRotationInterpolationSpeed <= 0.0f)
	{
		RefreshActorRotationInstant(TargetYawAngle, ETeleportType::TeleportPhysics);
	}
	else
	{
		RefreshTargetYawAngle(TargetYawAngle, false);
	}

	GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);

	// Force set locomotion action without waiting for the UAlsAnimNotifyState_SetLocomotionAction animation notify to start.

	SetLocomotionAction(EAlsLocomotionAction::Rolling);
}
