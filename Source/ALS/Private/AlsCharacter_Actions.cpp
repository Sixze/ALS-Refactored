#include "AlsCharacter.h"

#include "AlsAnimationInstance.h"
#include "AlsCharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/NetConnection.h"
#include "Engine/SkeletalMesh.h"
#include "Net/Core/PushModel/PushModel.h"
#include "RootMotionSources/AlsRootMotionSource_Mantling.h"
#include "Settings/AlsCharacterSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsLog.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"

void AAlsCharacter::StartRolling(const float PlayRate)
{
	if (LocomotionMode == AlsLocomotionModeTags::Grounded)
	{
		StartRolling(PlayRate, Settings->Rolling.bRotateToInputOnStart && LocomotionState.bHasInput
			                       ? LocomotionState.InputYawAngle
			                       : UE_REAL_TO_FLOAT(FRotator::NormalizeAxis(GetActorRotation().Yaw)));
	}
}

bool AAlsCharacter::IsRollingAllowedToStart(const UAnimMontage* Montage) const
{
	return !LocomotionAction.IsValid() ||
	       (LocomotionAction == AlsLocomotionActionTags::Rolling &&
	        !GetMesh()->GetAnimInstance()->Montage_IsPlaying(Montage));
}

void AAlsCharacter::StartRolling(const float PlayRate, const float TargetYawAngle)
{
	if (GetLocalRole() <= ROLE_SimulatedProxy)
	{
		return;
	}

	auto* Montage{SelectRollMontage()};

	if (!ALS_ENSURE(IsValid(Montage)) || !IsRollingAllowedToStart(Montage))
	{
		return;
	}

	const auto InitialYawAngle{UE_REAL_TO_FLOAT(FRotator::NormalizeAxis(GetActorRotation().Yaw))};

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartRolling(Montage, PlayRate, InitialYawAngle, TargetYawAngle);
	}
	else
	{
		GetCharacterMovement()->FlushServerMoves();

		StartRollingImplementation(Montage, PlayRate, InitialYawAngle, TargetYawAngle);
		ServerStartRolling(Montage, PlayRate, InitialYawAngle, TargetYawAngle);
	}
}

UAnimMontage* AAlsCharacter::SelectRollMontage_Implementation()
{
	return Settings->Rolling.Montage;
}

void AAlsCharacter::ServerStartRolling_Implementation(UAnimMontage* Montage, const float PlayRate,
                                                      const float InitialYawAngle, const float TargetYawAngle)
{
	if (IsRollingAllowedToStart(Montage))
	{
		MulticastStartRolling(Montage, PlayRate, InitialYawAngle, TargetYawAngle);
		ForceNetUpdate();
	}
}

void AAlsCharacter::MulticastStartRolling_Implementation(UAnimMontage* Montage, const float PlayRate,
                                                         const float InitialYawAngle, const float TargetYawAngle)
{
	StartRollingImplementation(Montage, PlayRate, InitialYawAngle, TargetYawAngle);
}

void AAlsCharacter::StartRollingImplementation(UAnimMontage* Montage, const float PlayRate,
                                               const float InitialYawAngle, const float TargetYawAngle)
{
	if (IsRollingAllowedToStart(Montage) && GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate))
	{
		RollingState.TargetYawAngle = TargetYawAngle;

		RefreshRotationInstant(InitialYawAngle);

		SetLocomotionAction(AlsLocomotionActionTags::Rolling);
	}
}

void AAlsCharacter::RefreshRolling(const float DeltaTime)
{
	if (GetLocalRole() <= ROLE_SimulatedProxy ||
	    GetMesh()->GetAnimInstance()->RootMotionMode <= ERootMotionMode::IgnoreRootMotion)
	{
		// Refresh rolling physics here because AAlsCharacter::PhysicsRotation()
		// won't be called on simulated proxies or with ignored root motion.

		RefreshRollingPhysics(DeltaTime);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AAlsCharacter::RefreshRollingPhysics(const float DeltaTime)
{
	if (LocomotionAction != AlsLocomotionActionTags::Rolling)
	{
		return;
	}

	auto TargetRotation{GetCharacterMovement()->UpdatedComponent->GetComponentRotation()};

	if (Settings->Rolling.RotationInterpolationSpeed <= 0.0f)
	{
		TargetRotation.Yaw = RollingState.TargetYawAngle;

		GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, TargetRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		TargetRotation.Yaw = UAlsMath::ExponentialDecayAngle(UE_REAL_TO_FLOAT(FRotator::NormalizeAxis(TargetRotation.Yaw)),
		                                                     RollingState.TargetYawAngle, DeltaTime,
		                                                     Settings->Rolling.RotationInterpolationSpeed);

		GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, TargetRotation, false);
	}
}

bool AAlsCharacter::StartMantlingGrounded()
{
	return LocomotionMode == AlsLocomotionModeTags::Grounded &&
	       StartMantling(Settings->Mantling.GroundedTrace);
}

bool AAlsCharacter::StartMantlingInAir()
{
	return LocomotionMode == AlsLocomotionModeTags::InAir && IsLocallyControlled() &&
	       StartMantling(Settings->Mantling.InAirTrace);
}

bool AAlsCharacter::IsMantlingAllowedToStart_Implementation() const
{
	return !LocomotionAction.IsValid();
}

bool AAlsCharacter::StartMantling(const FAlsMantlingTraceSettings& TraceSettings)
{
	if (!Settings->Mantling.bAllowMantling || GetLocalRole() <= ROLE_SimulatedProxy || !IsMantlingAllowedToStart())
	{
		return false;
	}

	const auto ActorLocation{GetActorLocation()};
	const auto ActorYawAngle{UE_REAL_TO_FLOAT(FRotator::NormalizeAxis(GetActorRotation().Yaw))};

	float ForwardTraceAngle;
	if (LocomotionState.bHasSpeed)
	{
		ForwardTraceAngle = LocomotionState.bHasInput
			                    ? LocomotionState.VelocityYawAngle +
			                      FMath::ClampAngle(LocomotionState.InputYawAngle - LocomotionState.VelocityYawAngle,
			                                        -Settings->Mantling.MaxReachAngle, Settings->Mantling.MaxReachAngle)
			                    : LocomotionState.VelocityYawAngle;
	}
	else
	{
		ForwardTraceAngle = LocomotionState.bHasInput ? LocomotionState.InputYawAngle : ActorYawAngle;
	}

	const auto ForwardTraceDeltaAngle{FRotator3f::NormalizeAxis(ForwardTraceAngle - ActorYawAngle)};
	if (FMath::Abs(ForwardTraceDeltaAngle) > Settings->Mantling.TraceAngleThreshold)
	{
		return false;
	}

	const auto ForwardTraceDirection{
		UAlsMath::AngleToDirectionXY(
			ActorYawAngle + FMath::ClampAngle(ForwardTraceDeltaAngle, -Settings->Mantling.MaxReachAngle, Settings->Mantling.MaxReachAngle))
	};

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebugForActor(this, UAlsConstants::MantlingDebugDisplayName())};
#endif

	const auto* Capsule{GetCapsuleComponent()};

	const auto CapsuleScale{Capsule->GetComponentScale().Z};
	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	const FVector CapsuleBottomLocation{ActorLocation.X, ActorLocation.Y, ActorLocation.Z - CapsuleHalfHeight};

	const auto TraceCapsuleRadius{CapsuleRadius - 1.0f};

	const auto LedgeHeightDelta{UE_REAL_TO_FLOAT((TraceSettings.LedgeHeight.GetMax() - TraceSettings.LedgeHeight.GetMin()) * CapsuleScale)};

	// Trace forward to find an object the character cannot walk on.

	static const FName ForwardTraceTag{FString::Printf(TEXT("%hs (Forward Trace)"), __FUNCTION__)};

	auto ForwardTraceStart{CapsuleBottomLocation - ForwardTraceDirection * CapsuleRadius};
	ForwardTraceStart.Z += (TraceSettings.LedgeHeight.X + TraceSettings.LedgeHeight.Y) *
		0.5f * CapsuleScale - UCharacterMovementComponent::MAX_FLOOR_DIST;

	auto ForwardTraceEnd{ForwardTraceStart + ForwardTraceDirection * (CapsuleRadius + (TraceSettings.ReachDistance + 1.0f) * CapsuleScale)};

	const auto ForwardTraceCapsuleHalfHeight{LedgeHeightDelta * 0.5f};

	FHitResult ForwardTraceHit;
	GetWorld()->SweepSingleByChannel(ForwardTraceHit, ForwardTraceStart, ForwardTraceEnd,
	                                 FQuat::Identity, Settings->Mantling.MantlingTraceChannel,
	                                 FCollisionShape::MakeCapsule(TraceCapsuleRadius, ForwardTraceCapsuleHalfHeight),
	                                 {ForwardTraceTag, false, this}, Settings->Mantling.MantlingTraceResponses);

	auto* TargetPrimitive{ForwardTraceHit.GetComponent()};

	if (!ForwardTraceHit.IsValidBlockingHit() ||
	    !IsValid(TargetPrimitive) ||
	    TargetPrimitive->GetComponentVelocity().SizeSquared() > FMath::Square(Settings->Mantling.TargetPrimitiveSpeedThreshold) ||
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

	const auto TargetDirection{-ForwardTraceHit.ImpactNormal.GetSafeNormal2D()};

	// Trace downward from the first trace's impact point and determine if the hit location is walkable.

	static const FName DownwardTraceTag{FString::Printf(TEXT("%hs (Downward Trace)"), __FUNCTION__)};

	const FVector2D TargetLocationOffset{TargetDirection * (TraceSettings.TargetLocationOffset * CapsuleScale)};

	const FVector DownwardTraceStart{
		ForwardTraceHit.ImpactPoint.X + TargetLocationOffset.X,
		ForwardTraceHit.ImpactPoint.Y + TargetLocationOffset.Y,
		CapsuleBottomLocation.Z + LedgeHeightDelta + 2.5f * TraceCapsuleRadius + UCharacterMovementComponent::MIN_FLOOR_DIST
	};

	const FVector DownwardTraceEnd{
		DownwardTraceStart.X,
		DownwardTraceStart.Y,
		CapsuleBottomLocation.Z +
		TraceSettings.LedgeHeight.GetMin() * CapsuleScale + TraceCapsuleRadius - UCharacterMovementComponent::MAX_FLOOR_DIST
	};

	FHitResult DownwardTraceHit;
	GetWorld()->SweepSingleByChannel(DownwardTraceHit, DownwardTraceStart, DownwardTraceEnd, FQuat::Identity,
	                                 Settings->Mantling.MantlingTraceChannel, FCollisionShape::MakeSphere(TraceCapsuleRadius),
	                                 {DownwardTraceTag, false, this}, Settings->Mantling.MantlingTraceResponses);

	const auto SlopeAngleCos{UE_REAL_TO_FLOAT(DownwardTraceHit.ImpactNormal.Z)};

	// The approximate slope angle is used in situations where the normal slope angle cannot convey
	// the true nature of the surface slope, for example, for a 45 degree staircase the slope
	// angle will always be 90 degrees, while the approximate slope angle will be ~45 degrees.

	auto ApproximateSlopeNormal{DownwardTraceHit.Location - DownwardTraceHit.ImpactPoint};
	ApproximateSlopeNormal.Normalize();

	const auto ApproximateSlopeAngleCos{UE_REAL_TO_FLOAT(ApproximateSlopeNormal.Z)};

	if (SlopeAngleCos < Settings->Mantling.SlopeAngleThresholdCos ||
	    ApproximateSlopeAngleCos < Settings->Mantling.SlopeAngleThresholdCos ||
	    !GetCharacterMovement()->IsWalkable(DownwardTraceHit))
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

	// Check that there is enough free space for the capsule at the target location.

	static const FName TargetLocationTraceTag{FString::Printf(TEXT("%hs (Target Location Overlap)"), __FUNCTION__)};

	const FVector TargetLocation{
		DownwardTraceHit.Location.X,
		DownwardTraceHit.Location.Y,
		DownwardTraceHit.ImpactPoint.Z + UCharacterMovementComponent::MIN_FLOOR_DIST
	};

	const FVector TargetCapsuleLocation{TargetLocation.X, TargetLocation.Y, TargetLocation.Z + CapsuleHalfHeight};

	if (GetWorld()->OverlapBlockingTestByChannel(TargetCapsuleLocation, FQuat::Identity, Settings->Mantling.MantlingTraceChannel,
	                                             FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
	                                             {TargetLocationTraceTag, false, this}, Settings->Mantling.MantlingTraceResponses))
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

			DrawDebugCapsule(GetWorld(), TargetCapsuleLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity,
			                 FColor::Red, false, TraceSettings.bDrawFailedTraces ? 10.0f : 0.0f);
		}
#endif

		return false;
	}

	// Perform additional overlap at the approximate start location to
	// ensure there are no vertical obstacles on the path, such as a ceiling.

	static const FName StartLocationTraceTag{FString::Printf(TEXT("%hs (Start Location Overlap)"), __FUNCTION__)};

	const FVector2D StartLocationOffset{TargetDirection * (TraceSettings.StartLocationOffset * CapsuleScale)};

	const FVector StartLocation{
		ForwardTraceHit.ImpactPoint.X - StartLocationOffset.X,
		ForwardTraceHit.ImpactPoint.Y - StartLocationOffset.Y,
		(DownwardTraceHit.Location.Z + DownwardTraceEnd.Z) * 0.5f
	};

	const auto StartLocationTraceCapsuleHalfHeight{(DownwardTraceHit.Location.Z - DownwardTraceEnd.Z) * 0.5f + TraceCapsuleRadius};

	if (GetWorld()->OverlapBlockingTestByChannel(StartLocation, FQuat::Identity, Settings->Mantling.MantlingTraceChannel,
	                                             FCollisionShape::MakeCapsule(TraceCapsuleRadius, StartLocationTraceCapsuleHalfHeight),
	                                             {StartLocationTraceTag, false, this}, Settings->Mantling.MantlingTraceResponses))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit,
			                                                    {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);

			UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd, TraceCapsuleRadius,
			                                        false, DownwardTraceHit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f},
			                                        TraceSettings.bDrawFailedTraces ? 7.5f : 0.0f);

			DrawDebugCapsule(GetWorld(), StartLocation, StartLocationTraceCapsuleHalfHeight, TraceCapsuleRadius, FQuat::Identity,
			                 FLinearColor{1.0f, 0.5f, 0.0f}.ToFColor(true), false, TraceSettings.bDrawFailedTraces ? 10.0f : 0.0f);
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

	const auto TargetRotation{TargetDirection.ToOrientationQuat()};

	FAlsMantlingParameters Parameters;

	Parameters.TargetPrimitive = TargetPrimitive;
	Parameters.MantlingHeight = UE_REAL_TO_FLOAT((TargetLocation.Z - CapsuleBottomLocation.Z) / CapsuleScale);

	// Determine the mantling type by checking the movement mode and mantling height.

	Parameters.MantlingType = LocomotionMode != AlsLocomotionModeTags::Grounded
		                          ? EAlsMantlingType::InAir
		                          : Parameters.MantlingHeight > Settings->Mantling.MantlingHighHeightThreshold
		                          ? EAlsMantlingType::High
		                          : EAlsMantlingType::Low;

	// If the target primitive can't move, then use world coordinates to save
	// some performance by skipping some coordinate space transformations later.

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

	const auto* MantlingSettings{SelectMantlingSettings(Parameters.MantlingType)};

	if (!ALS_ENSURE(IsValid(MantlingSettings)) || !ALS_ENSURE(IsValid(MantlingSettings->Montage)))
	{
		return;
	}

	const auto StartTime{CalculateMantlingStartTime(MantlingSettings, Parameters.MantlingHeight)};
	const auto Duration{MantlingSettings->Montage->GetPlayLength() - StartTime};
	const auto PlayRate{MantlingSettings->Montage->RateScale};

	const auto TargetAnimationLocation{UAlsUtility::ExtractLastRootTransformFromMontage(MantlingSettings->Montage).GetLocation()};

	if (FMath::IsNearlyZero(TargetAnimationLocation.Z))
	{
		UE_LOG(LogAls, Warning, TEXT("Can't start mantling! The %s animation montage has incorrect root motion,")
		       TEXT(" the final vertical location of the character must be non-zero!"), *MantlingSettings->Montage->GetName());
		return;
	}

	// Calculate actor offsets (offsets between actor and target transform).

	const auto bUseRelativeLocation{MovementBaseUtility::UseRelativeLocation(Parameters.TargetPrimitive.Get())};
	const auto TargetRelativeRotation{Parameters.TargetRelativeRotation.GetNormalized()};

	const auto TargetTransform{
		bUseRelativeLocation
			? FTransform{
				TargetRelativeRotation, Parameters.TargetRelativeLocation,
				Parameters.TargetPrimitive->GetComponentScale()
			}.GetRelativeTransformReverse(Parameters.TargetPrimitive->GetComponentTransform())
			: FTransform{TargetRelativeRotation, Parameters.TargetRelativeLocation}
	};

	const auto ActorFeetLocationOffset{GetCharacterMovement()->GetActorFeetLocation() - TargetTransform.GetLocation()};
	const auto ActorRotationOffset{TargetTransform.GetRotation().Inverse() * GetActorQuat()};

	// Reset network smoothing.

	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;

	GetMesh()->SetRelativeLocationAndRotation(GetBaseTranslationOffset(),
	                                          GetMesh()->IsUsingAbsoluteRotation()
		                                          ? GetActorQuat() * GetBaseRotationOffset()
		                                          : GetBaseRotationOffset(), false, nullptr, ETeleportType::TeleportPhysics);

	// Clear the character movement mode and set the locomotion action to mantling.

	GetCharacterMovement()->SetMovementMode(MOVE_Custom);
	AlsCharacterMovement->SetMovementModeLocked(true);

	GetCharacterMovement()->SetBase(Parameters.TargetPrimitive.Get());

	// Apply mantling root motion.

	const auto RootMotionSource{MakeShared<FAlsRootMotionSource_Mantling>()};
	RootMotionSource->InstanceName = __FUNCTION__;
	RootMotionSource->Duration = Duration / PlayRate;
	RootMotionSource->MantlingSettings = MantlingSettings;
	RootMotionSource->TargetPrimitive = Parameters.TargetPrimitive;
	RootMotionSource->TargetRelativeLocation = Parameters.TargetRelativeLocation;
	RootMotionSource->TargetRelativeRotation = TargetRelativeRotation;
	RootMotionSource->ActorFeetLocationOffset = ActorFeetLocationOffset;
	RootMotionSource->ActorRotationOffset = ActorRotationOffset.Rotator();
	RootMotionSource->TargetAnimationLocation = TargetAnimationLocation;
	RootMotionSource->MontageStartTime = StartTime;

	MantlingState.RootMotionSourceId = GetCharacterMovement()->ApplyRootMotionSource(RootMotionSource);

	// Play the animation montage if valid.

	if (GetMesh()->GetAnimInstance()->Montage_Play(MantlingSettings->Montage, 1.0f,
	                                               EMontagePlayReturnType::MontageLength,
	                                               StartTime, false))
	{
		SetLocomotionAction(AlsLocomotionActionTags::Mantling);
	}

	OnMantlingStarted(Parameters);
}

UAlsMantlingSettings* AAlsCharacter::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return nullptr;
}

float AAlsCharacter::CalculateMantlingStartTime(const UAlsMantlingSettings* MantlingSettings, const float MantlingHeight) const
{
	if (!MantlingSettings->bAutoCalculateStartTime)
	{
		return FMath::GetMappedRangeValueClamped(MantlingSettings->StartTimeReferenceHeight, MantlingSettings->StartTime, MantlingHeight);
	}

	// https://landelare.github.io/2022/05/15/climbing-with-root-motion.html

	const auto* Montage{MantlingSettings->Montage.Get()};
	if (!IsValid(Montage))
	{
		return 0.0f;
	}

	const auto MontageFrameRate{1.0f / Montage->GetSamplingFrameRate().AsDecimal()};

	auto SearchStartTime{0.0f};
	auto SearchEndTime{Montage->GetPlayLength()};

	const auto SearchStartLocationZ{UAlsUtility::ExtractRootTransformFromMontage(Montage, SearchStartTime).GetTranslation().Z};
	const auto SearchEndLocationZ{UAlsUtility::ExtractRootTransformFromMontage(Montage, SearchEndTime).GetTranslation().Z};

	// Find the vertical distance the character has already moved.

	const auto TargetLocationZ{FMath::Max(0.0f, SearchEndLocationZ - MantlingHeight)};

	// Perform a binary search to find the time when the character is at the target vertical distance.

	static constexpr auto MaxLocationSearchTolerance{1.0f};

	if (FMath::IsNearlyEqual(SearchStartLocationZ, TargetLocationZ, MaxLocationSearchTolerance))
	{
		return SearchStartTime;
	}

	while (true)
	{
		const auto Time{(SearchStartTime + SearchEndTime) * 0.5f};
		const auto LocationZ{UAlsUtility::ExtractRootTransformFromMontage(Montage, Time).GetTranslation().Z};

		// Stop the search if a close enough location has been found or if
		// the search interval is less than the animation montage frame rate.

		if (FMath::IsNearlyEqual(LocationZ, TargetLocationZ, MaxLocationSearchTolerance) ||
		    SearchEndTime - SearchStartTime <= MontageFrameRate)
		{
			return Time;
		}

		if (LocationZ < TargetLocationZ)
		{
			SearchStartTime = Time;
		}
		else
		{
			SearchEndTime = Time;
		}
	}
}

void AAlsCharacter::OnMantlingStarted_Implementation(const FAlsMantlingParameters& Parameters) {}

void AAlsCharacter::RefreshMantling()
{
	if (MantlingState.RootMotionSourceId <= 0)
	{
		return;
	}

	if (LocomotionAction != AlsLocomotionActionTags::Mantling)
	{
		StopMantling();
		return;
	}

	if (GetCharacterMovement()->MovementMode != MOVE_Custom)
	{
		StopMantling(true);
		return;
	}

	const auto* RootMotionSource{
		StaticCastSharedPtr<FAlsRootMotionSource_Mantling>(GetCharacterMovement()
			->GetRootMotionSourceByID(MantlingState.RootMotionSourceId)).Get()
	};

	if (RootMotionSource != nullptr && !RootMotionSource->TargetPrimitive.IsValid())
	{
		StopMantling(true);

		if (Settings->Mantling.bStartRagdollingOnTargetPrimitiveDestruction)
		{
			StartRagdolling();
		}
	}
}

void AAlsCharacter::StopMantling(const bool bStopMontage)
{
	if (MantlingState.RootMotionSourceId <= 0)
	{
		return;
	}

	auto* RootMotionSource{
		StaticCastSharedPtr<FAlsRootMotionSource_Mantling>(GetCharacterMovement()
			->GetRootMotionSourceByID(MantlingState.RootMotionSourceId)).Get()
	};

	if (RootMotionSource != nullptr)
	{
		RootMotionSource->Status.SetFlag(ERootMotionSourceStatusFlags::MarkedForRemoval);
	}

	MantlingState.RootMotionSourceId = 0;

	if (bStopMontage && RootMotionSource != nullptr)
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(Settings->Mantling.BlendOutDuration, RootMotionSource->MantlingSettings->Montage);
	}

	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Exponential;

	AlsCharacterMovement->SetMovementModeLocked(false);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	OnMantlingEnded();

	ForceNetUpdate();
}

void AAlsCharacter::OnMantlingEnded_Implementation() {}

bool AAlsCharacter::IsRagdollingAllowedToStart() const
{
	return LocomotionAction != AlsLocomotionActionTags::Ragdolling;
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

	GetMesh()->bUpdateJointsFromAnimation = true; // Required for the flail animation to work properly.

	if (!GetMesh()->IsRunningParallelEvaluation() && GetMesh()->GetBoneSpaceTransforms().Num() > 0)
	{
		GetMesh()->UpdateRBJointMotors();
	}

	// Stop any active montages.

	static constexpr auto BlendOutDuration{0.2f};

	GetMesh()->GetAnimInstance()->Montage_Stop(BlendOutDuration);

	// Disable movement corrections and reset network smoothing.

	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;
	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = true;

	// Detach the mesh so that character transformation changes will not affect it in any way.

	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	// Disable capsule collision and enable mesh physics simulation.

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);

	const auto* PelvisBody{GetMesh()->GetBodyInstance(UAlsConstants::PelvisBoneName())};
	FVector PelvisLocation;

	FPhysicsCommand::ExecuteRead(PelvisBody->ActorHandle, [this, &PelvisLocation](const FPhysicsActorHandle& ActorHandle)
	{
		PelvisLocation = FPhysicsInterface::GetTransform_AssumesLocked(ActorHandle, true).GetLocation();
		RagdollingState.Velocity = FPhysicsInterface::GetLinearVelocity_AssumesLocked(ActorHandle);
	});

	RagdollingState.PullForce = 0.0f;

	if (Settings->Ragdolling.bLimitInitialRagdollSpeed)
	{
		// Limit the ragdoll's speed for a few frames, because for some unclear reason,
		// it can get a much higher initial speed than the character's last speed.
		// TODO Find a better solution or wait for a fix in future engine versions.

		static constexpr auto MinSpeedLimit{200.0f};

		RagdollingState.SpeedLimitFrameTimeRemaining = 8;
		RagdollingState.SpeedLimit = FMath::Max(MinSpeedLimit, LocomotionState.Velocity.Size());

		LimitRagdollSpeed();
	}

	if (IsLocallyControlled() || (GetLocalRole() >= ROLE_Authority && !IsValid(GetController())))
	{
		SetRagdollTargetLocation(PelvisLocation);
	}

	// Clear the character movement mode and set the locomotion action to ragdolling.

	GetCharacterMovement()->SetMovementMode(MOVE_None);
	AlsCharacterMovement->SetMovementModeLocked(true);

	SetLocomotionAction(AlsLocomotionActionTags::Ragdolling);

	OnRagdollingStarted();
}

void AAlsCharacter::OnRagdollingStarted_Implementation() {}

void AAlsCharacter::SetRagdollTargetLocation(const FVector& NewTargetLocation)
{
	if (RagdollTargetLocation != NewTargetLocation)
	{
		RagdollTargetLocation = NewTargetLocation;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RagdollTargetLocation, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetRagdollTargetLocation(RagdollTargetLocation);
		}
	}
}

void AAlsCharacter::ServerSetRagdollTargetLocation_Implementation(const FVector_NetQuantize& NewTargetLocation)
{
	SetRagdollTargetLocation(NewTargetLocation);
}

void AAlsCharacter::RefreshRagdolling(const float DeltaTime)
{
	if (LocomotionAction != AlsLocomotionActionTags::Ragdolling)
	{
		return;
	}

	// Since we are dealing with physics here, we should not use functions such as USkinnedMeshComponent::GetSocketTransform() as
	// they may return an incorrect result in situations like when the animation blueprint is not ticking or when URO is enabled.

	const auto* PelvisBody{GetMesh()->GetBodyInstance(UAlsConstants::PelvisBoneName())};
	FVector PelvisLocation;

	FPhysicsCommand::ExecuteRead(PelvisBody->ActorHandle, [this, &PelvisLocation](const FPhysicsActorHandle& ActorHandle)
	{
		PelvisLocation = FPhysicsInterface::GetTransform_AssumesLocked(ActorHandle, true).GetLocation();
		RagdollingState.Velocity = FPhysicsInterface::GetLinearVelocity_AssumesLocked(ActorHandle);
	});

	const auto bLocallyControlled{IsLocallyControlled() || (GetLocalRole() >= ROLE_Authority && !IsValid(GetController()))};

	if (bLocallyControlled)
	{
		SetRagdollTargetLocation(PelvisLocation);
	}

	// Prevent the capsule from going through the ground when the ragdoll is lying on the ground.

	// While we could get rid of the line trace here and just use RagdollTargetLocation
	// as the character's location, we don't do that because the camera depends on the
	// capsule's bottom location, so its removal will cause the camera to behave erratically.

	bool bGrounded;
	SetActorLocation(RagdollTraceGround(bGrounded), false, nullptr, ETeleportType::TeleportPhysics);

	// Zero target location means that it hasn't been replicated yet, so we can't apply the logic below.

	if (!bLocallyControlled && !RagdollTargetLocation.IsZero())
	{
		// Apply ragdoll location corrections.

		static constexpr auto PullForce{750.0f};
		static constexpr auto InterpolationSpeed{0.6f};

		RagdollingState.PullForce = FMath::FInterpTo(RagdollingState.PullForce, PullForce, DeltaTime, InterpolationSpeed);

		const auto HorizontalSpeedSquared{RagdollingState.Velocity.SizeSquared2D()};

		const auto PullForceBoneName{
			HorizontalSpeedSquared > FMath::Square(300.0f) ? UAlsConstants::Spine03BoneName() : UAlsConstants::PelvisBoneName()
		};

		auto* PullForceBody{GetMesh()->GetBodyInstance(PullForceBoneName)};

		FPhysicsCommand::ExecuteWrite(PullForceBody->ActorHandle, [this](const FPhysicsActorHandle& ActorHandle)
		{
			if (!FPhysicsInterface::IsRigidBody(ActorHandle))
			{
				return;
			}

			const auto PullForceVector{
				RagdollTargetLocation - FPhysicsInterface::GetTransform_AssumesLocked(ActorHandle, true).GetLocation()
			};

			static constexpr auto MinPullForceDistance{5.0f};
			static constexpr auto MaxPullForceDistance{50.0f};

			if (PullForceVector.SizeSquared() > FMath::Square(MinPullForceDistance))
			{
				FPhysicsInterface::AddForce_AssumesLocked(
					ActorHandle, PullForceVector.GetClampedToMaxSize(MaxPullForceDistance) * RagdollingState.PullForce, true, true);
			}
		});
	}

	// Use the speed to scale ragdoll joint strength for physical animation.

	static constexpr auto ReferenceSpeed{1000.0f};
	static constexpr auto Stiffness{25000.0f};

	const auto SpeedAmount{UAlsMath::Clamp01(UE_REAL_TO_FLOAT(RagdollingState.Velocity.Size() / ReferenceSpeed))};

	GetMesh()->SetAllMotorsAngularDriveParams(SpeedAmount * Stiffness, 0.0f, 0.0f);

	// Limit the speed of ragdoll bodies.

	if (RagdollingState.SpeedLimitFrameTimeRemaining > 0)
	{
		RagdollingState.SpeedLimitFrameTimeRemaining -= 1;

		LimitRagdollSpeed();
	}
}

FVector AAlsCharacter::RagdollTraceGround(bool& bGrounded) const
{
	const auto CapsuleHalfHeight{GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};

	const auto TraceStart{!RagdollTargetLocation.IsZero() ? FVector{RagdollTargetLocation} : GetActorLocation()};
	const FVector TraceEnd{TraceStart.X, TraceStart.Y, TraceStart.Z - CapsuleHalfHeight};

	FHitResult Hit;
	bGrounded = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, Settings->Ragdolling.GroundTraceChannel,
	                                                 {__FUNCTION__, false, this}, Settings->Ragdolling.GroundTraceResponses);

	return {
		TraceStart.X, TraceStart.Y,
		bGrounded ? Hit.ImpactPoint.Z + CapsuleHalfHeight + UCharacterMovementComponent::MIN_FLOOR_DIST : TraceStart.Z
	};
}

void AAlsCharacter::LimitRagdollSpeed() const
{
	GetMesh()->ForEachBodyBelow(NAME_None, true, false, [this](FBodyInstance* Body)
	{
		FPhysicsCommand::ExecuteWrite(Body->ActorHandle, [this](const FPhysicsActorHandle& ActorHandle)
		{
			if (!FPhysicsInterface::IsRigidBody(ActorHandle))
			{
				return;
			}

			auto Velocity{FPhysicsInterface::GetLinearVelocity_AssumesLocked(ActorHandle)};
			if (Velocity.SizeSquared() <= FMath::Square(RagdollingState.SpeedLimit))
			{
				return;
			}

			Velocity.Normalize();
			Velocity *= RagdollingState.SpeedLimit;

			FPhysicsInterface::SetLinearVelocity_AssumesLocked(ActorHandle, Velocity);
		});
	});
}

bool AAlsCharacter::IsRagdollingAllowedToStop() const
{
	return LocomotionAction == AlsLocomotionActionTags::Ragdolling;
}

bool AAlsCharacter::StopRagdolling()
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

	auto& FinalRagdollPose{AnimationInstance->SnapshotFinalRagdollPose()};

	const auto PelvisTransform{GetMesh()->GetSocketTransform(UAlsConstants::PelvisBoneName())};
	const auto PelvisRotation{PelvisTransform.Rotator()};

	// Disable mesh physics simulation and enable capsule collision.

	GetMesh()->bUpdateJointsFromAnimation = false;

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Exponential;
	GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = false;

	bool bGrounded;
	const auto NewActorLocation{RagdollTraceGround(bGrounded)};

	// Determine whether the ragdoll is facing upward or downward and set the actor rotation accordingly.

	const auto bRagdollFacingUpward{FRotator::NormalizeAxis(PelvisRotation.Roll) <= 0.0f};

	auto NewActorRotation{GetActorRotation()};
	NewActorRotation.Yaw = bRagdollFacingUpward ? PelvisRotation.Yaw - 180.0f : PelvisRotation.Yaw;

	SetActorLocationAndRotation(NewActorLocation, NewActorRotation, false, nullptr, ETeleportType::TeleportPhysics);

	// Attach the mesh back and restore its default relative location.

	const auto& ActorTransform{GetActorTransform()};

	GetMesh()->SetWorldLocationAndRotationNoPhysics(ActorTransform.TransformPositionNoScale(GetBaseTranslationOffset()),
	                                                ActorTransform.TransformRotation(GetBaseRotationOffset()).Rotator());

	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);

	if (GetMesh()->ShouldUseUpdateRateOptimizations())
	{
		// Disable URO for one frame to force the animation blueprint to update and get rid of the incorrect mesh pose.

		GetMesh()->bEnableUpdateRateOptimizations = false;

		GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]
		{
			ensure(!GetMesh()->bEnableUpdateRateOptimizations);
			GetMesh()->bEnableUpdateRateOptimizations = true;
		}));
	}

	// Restore the pelvis transform to the state it was in before we changed
	// the character and mesh transforms to keep its world transform unchanged.

	const auto& ReferenceSkeleton{GetMesh()->GetSkeletalMeshAsset()->GetRefSkeleton()};

	const auto PelvisBoneIndex{ReferenceSkeleton.FindBoneIndex(UAlsConstants::PelvisBoneName())};
	if (ALS_ENSURE(PelvisBoneIndex >= 0))
	{
		// We expect the pelvis bone to be the root bone or attached to it, so we can safely use the mesh transform here.
		FinalRagdollPose.LocalTransforms[PelvisBoneIndex] = PelvisTransform.GetRelativeTransform(GetMesh()->GetComponentTransform());
	}

	SetRagdollTargetLocation(FVector::ZeroVector);

	// If the ragdoll is on the ground, set the movement mode to walking and play a get up montage. If not, set
	// the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.

	AlsCharacterMovement->SetMovementModeLocked(false);

	if (bGrounded)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = RagdollingState.Velocity;
	}

	SetLocomotionAction(FGameplayTag::EmptyTag);

	OnRagdollingEnded();

	if (bGrounded &&
	    GetMesh()->GetAnimInstance()->Montage_Play(SelectGetUpMontage(bRagdollFacingUpward), 1.0f,
	                                               EMontagePlayReturnType::MontageLength, 0.0f, true))
	{
		AlsCharacterMovement->SetInputBlocked(true);

		SetLocomotionAction(AlsLocomotionActionTags::GettingUp);
	}
}

UAnimMontage* AAlsCharacter::SelectGetUpMontage_Implementation(const bool bRagdollFacingUpward)
{
	return bRagdollFacingUpward ? Settings->Ragdolling.GetUpBackMontage : Settings->Ragdolling.GetUpFrontMontage;
}

void AAlsCharacter::OnRagdollingEnded_Implementation() {}
