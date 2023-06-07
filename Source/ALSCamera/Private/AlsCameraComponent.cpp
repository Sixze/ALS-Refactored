#include "AlsCameraComponent.h"

#include "AlsCameraSettings.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/WorldSettings.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsCameraComponent)

UAlsCameraComponent::UAlsCameraComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = false;
	bHiddenInGame = true;
}

void UAlsCameraComponent::OnRegister()
{
	Character = Cast<ACharacter>(GetOwner());

	Super::OnRegister();
}

void UAlsCameraComponent::RegisterComponentTickFunctions(const bool bRegister)
{
	Super::RegisterComponentTickFunctions(bRegister);

	// Tick after the owner to have access to the most up-to-date character state.

	AddTickPrerequisiteActor(GetOwner());
}

void UAlsCameraComponent::Activate(const bool bReset)
{
	if (!bReset && !ShouldActivate())
	{
		Super::Activate(bReset);
		return;
	}

	Super::Activate(bReset);

	TickCamera(0.0f, false);
}

void UAlsCameraComponent::InitAnim(const bool bForceReinitialize)
{
	Super::InitAnim(bForceReinitialize);

	AnimationInstance = GetAnimInstance();
}

void UAlsCameraComponent::BeginPlay()
{
	ALS_ENSURE(IsValid(GetAnimInstance()));
	ALS_ENSURE(IsValid(Settings));
	ALS_ENSURE(IsValid(Character));

	Super::BeginPlay();
}

void UAlsCameraComponent::TickComponent(float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsValid(Settings) && Settings->bIgnoreTimeDilation)
	{
		// Use the previous global time dilation, as this frame's delta time may not yet be affected
		// by the current global time dilation, and thus unscaling will produce the wrong delta time.

		const auto TimeDilation{PreviousGlobalTimeDilation * GetOwner()->CustomTimeDilation};

		DeltaTime = TimeDilation > UE_SMALL_NUMBER ? DeltaTime / TimeDilation : GetWorld()->DeltaRealTimeSeconds;
	}

	PreviousGlobalTimeDilation = GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Skip camera tick until parallel animation evaluation completes.

	if (!IsRunningParallelEvaluation())
	{
		TickCamera(DeltaTime);
	}
}

void UAlsCameraComponent::CompleteParallelAnimationEvaluation(const bool bDoPostAnimationEvaluation)
{
	Super::CompleteParallelAnimationEvaluation(bDoPostAnimationEvaluation);

	TickCamera(GetAnimInstance()->GetDeltaSeconds());
}

FVector UAlsCameraComponent::GetFirstPersonCameraLocation() const
{
	return Character->GetMesh()->GetSocketLocation(Settings->FirstPerson.CameraSocketName);
}

FVector UAlsCameraComponent::GetThirdPersonPivotLocation() const
{
	return (Character->GetMesh()->GetSocketLocation(Settings->ThirdPerson.FirstPivotSocketName) +
	        Character->GetMesh()->GetSocketLocation(Settings->ThirdPerson.SecondPivotSocketName)) * 0.5f;
}

FVector UAlsCameraComponent::GetThirdPersonTraceStartLocation() const
{
	return Character->GetMesh()->GetSocketLocation(bRightShoulder
		                                               ? Settings->ThirdPerson.TraceShoulderRightSocketName
		                                               : Settings->ThirdPerson.TraceShoulderLeftSocketName);
}

void UAlsCameraComponent::GetViewInfo(FMinimalViewInfo& ViewInfo) const
{
	ViewInfo.Location = CameraLocation;
	ViewInfo.Rotation = CameraRotation;
	ViewInfo.FOV = CameraFov;

	ViewInfo.PostProcessBlendWeight = IsValid(Settings) ? PostProcessWeight : 0.0f;

	if (ViewInfo.PostProcessBlendWeight > UE_SMALL_NUMBER)
	{
		ViewInfo.PostProcessSettings = Settings->PostProcess;
	}
}

void UAlsCameraComponent::TickCamera(const float DeltaTime, bool bAllowLag)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsCameraComponent::TickCamera()"), STAT_UAlsCameraComponent_TickCamera, STATGROUP_Als)

	if (!IsValid(GetAnimInstance()) || !IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	ALS_ENSURE_MESSAGE(!IsRunningParallelEvaluation(),
	                   TEXT("%hs should not be called during parallel animation evaluation, because accessing animation curves")
	                   TEXT(" causes the game thread to wait for the parallel task to complete, resulting in performance degradation."),
	                   __FUNCTION__);

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraShapes{
		UAlsUtility::ShouldDisplayDebugForActor(GetOwner(), UAlsCameraConstants::CameraShapesDebugDisplayName())
	};
#else
	const auto bDisplayDebugCameraShapes{false};
#endif

	// Refresh movement base.

	const auto& BasedMovement{Character->GetBasedMovement()};
	const auto bMovementBaseHasRelativeRotation{BasedMovement.HasRelativeRotation()};

	auto MovementBaseLocation{FVector::ZeroVector};
	auto MovementBaseRotation{FQuat::Identity};

	if (bMovementBaseHasRelativeRotation)
	{
		MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName,
		                                              MovementBaseLocation, MovementBaseRotation);
	}

	if (BasedMovement.MovementBase != MovementBasePrimitive || BasedMovement.BoneName != MovementBaseBoneName)
	{
		MovementBasePrimitive = BasedMovement.MovementBase;
		MovementBaseBoneName = BasedMovement.BoneName;

		if (bMovementBaseHasRelativeRotation)
		{
			const auto MovementBaseRotationInverse{MovementBaseRotation.Inverse()};

			PivotMovementBaseRelativeLagLocation = MovementBaseRotationInverse.RotateVector(PivotLagLocation - MovementBaseLocation);
			CameraMovementBaseRelativeRotation = MovementBaseRotationInverse * CameraRotation.Quaternion();
		}
		else
		{
			PivotMovementBaseRelativeLagLocation = FVector::ZeroVector;
			CameraMovementBaseRelativeRotation = FQuat::Identity;
		}
	}

	const auto CameraTargetRotation{Character->GetViewRotation()};

	const auto PreviousPivotTargetLocation{PivotTargetLocation};

	PivotTargetLocation = GetThirdPersonPivotLocation();

	const auto FirstPersonOverride{
		UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::FirstPersonOverrideCurveName()))
	};

	if (FAnimWeight::IsFullWeight(FirstPersonOverride))
	{
		// Skip other calculations if the character is fully in first-person mode.

		PivotLagLocation = PivotTargetLocation;
		PivotLocation = PivotTargetLocation;

		CameraLocation = GetFirstPersonCameraLocation();
		CameraRotation = CameraTargetRotation;
		CameraFov = Settings->FirstPerson.Fov;
		return;
	}

	// Force disable camera lag if the character was teleported.

	bAllowLag &= Settings->TeleportDistanceThreshold <= 0.0f ||
		FVector::DistSquared(PreviousPivotTargetLocation, PivotTargetLocation) <= FMath::Square(Settings->TeleportDistanceThreshold);

	// Calculate camera rotation.

	if (bMovementBaseHasRelativeRotation)
	{
		CameraRotation = (MovementBaseRotation * CameraMovementBaseRelativeRotation).Rotator();

		CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);

		CameraMovementBaseRelativeRotation = MovementBaseRotation.Inverse() * CameraRotation.Quaternion();
	}
	else
	{
		CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);
	}

	const FRotator CameraYawRotation{0.0f, CameraRotation.Yaw, 0.0f};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotTargetLocation, CameraYawRotation, 16.0f, FLinearColor::Green);
	}
#endif

	// Calculate pivot lag location. Get the pivot target location and interpolate using axis-independent lag for maximum control.

	if (bMovementBaseHasRelativeRotation)
	{
		PivotLagLocation = MovementBaseLocation + MovementBaseRotation.RotateVector(PivotMovementBaseRelativeLagLocation);

		PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation.Quaternion(), DeltaTime, bAllowLag);

		PivotMovementBaseRelativeLagLocation = MovementBaseRotation.UnrotateVector(PivotLagLocation - MovementBaseLocation);
	}
	else
	{
		PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation.Quaternion(), DeltaTime, bAllowLag);
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLagLocation, PivotTargetLocation,
		              FLinearColor{1.0f, 0.5f, 0.0f}.ToFColor(true),
		              false, 0.0f, 0, UAlsUtility::DrawLineThickness);

		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotLagLocation, CameraYawRotation, 16.0f, {1.0f, 0.5f, 0.0f});
	}
#endif

	// Calculate pivot location.

	const auto PivotOffset{CalculatePivotOffset()};

	PivotLocation = PivotLagLocation + PivotOffset;

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLocation, PivotLagLocation,
		              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
		              false, 0.0f, 0, UAlsUtility::DrawLineThickness);

		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotLocation, CameraYawRotation, 16.0f, {0.0f, 0.75f, 1.0f});
	}
#endif

	// Calculate target camera location.

	const auto CameraTargetLocation{PivotLocation + CalculateCameraOffset()};

	// Trace for an object between the camera and character to apply a corrective offset.

	const auto CameraResultLocation{CalculateCameraTrace(CameraTargetLocation, PivotOffset, DeltaTime, bAllowLag, TraceDistanceRatio)};

	if (!FAnimWeight::IsRelevant(FirstPersonOverride))
	{
		CameraLocation = CameraResultLocation;
		CameraFov = Settings->ThirdPerson.Fov;
	}
	else
	{
		CameraLocation = FMath::Lerp(CameraResultLocation, GetFirstPersonCameraLocation(), FirstPersonOverride);
		CameraFov = FMath::Lerp(Settings->ThirdPerson.Fov, Settings->FirstPerson.Fov, FirstPersonOverride);
	}
}

FRotator UAlsCameraComponent::CalculateCameraRotation(const FRotator& CameraTargetRotation,
                                                      const float DeltaTime, const bool bAllowLag) const
{
	if (!bAllowLag)
	{
		return CameraTargetRotation;
	}

	const auto RotationLag{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::RotationLagCurveName())};

	if (!Settings->bEnableCameraLagSubstepping ||
	    DeltaTime <= Settings->CameraLagSubstepping.LagSubstepDeltaTime ||
	    RotationLag <= 0.0f)
	{
		return UAlsMath::ExponentialDecay(CameraRotation, CameraTargetRotation, DeltaTime, RotationLag);
	}

	const auto CameraInitialRotation{CameraRotation};
	const auto SubstepRotationSpeed{(CameraTargetRotation - CameraInitialRotation).GetNormalized() * (1.0f / DeltaTime)};

	auto NewCameraRotation{CameraRotation};
	auto PreviousSubstepTime{0.0f};

	for (auto SubstepNumber{1};; SubstepNumber++)
	{
		const auto SubstepTime{SubstepNumber * Settings->CameraLagSubstepping.LagSubstepDeltaTime};
		if (SubstepTime < DeltaTime - UE_SMALL_NUMBER)
		{
			NewCameraRotation = FMath::RInterpTo(NewCameraRotation, CameraInitialRotation + SubstepRotationSpeed * SubstepTime,
			                                     SubstepTime - PreviousSubstepTime, RotationLag);

			PreviousSubstepTime = SubstepTime;
		}
		else
		{
			return FMath::RInterpTo(NewCameraRotation, CameraTargetRotation, DeltaTime - PreviousSubstepTime, RotationLag);
		}
	}
}

FVector UAlsCameraComponent::CalculatePivotLagLocation(const FQuat& CameraYawRotation, const float DeltaTime, const bool bAllowLag) const
{
	if (!bAllowLag)
	{
		return PivotTargetLocation;
	}

	const auto RelativePivotInitialLagLocation{CameraYawRotation.UnrotateVector(PivotLagLocation)};
	const auto RelativePivotTargetLocation{CameraYawRotation.UnrotateVector(PivotTargetLocation)};

	const auto LocationLagX{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagXCurveName())};
	const auto LocationLagY{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagYCurveName())};
	const auto LocationLagZ{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagZCurveName())};

	if (!Settings->bEnableCameraLagSubstepping ||
	    DeltaTime <= Settings->CameraLagSubstepping.LagSubstepDeltaTime ||
	    (LocationLagX <= 0.0f && LocationLagY <= 0.0f && LocationLagZ <= 0.0f))
	{
		return CameraYawRotation.RotateVector({
			UAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.X, RelativePivotTargetLocation.X, DeltaTime, LocationLagX),
			UAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.Y, RelativePivotTargetLocation.Y, DeltaTime, LocationLagY),
			UAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.Z, RelativePivotTargetLocation.Z, DeltaTime, LocationLagZ)
		});
	}

	const auto SubstepMovementSpeed{(RelativePivotTargetLocation - RelativePivotInitialLagLocation) / DeltaTime};

	auto RelativePivotLagLocation{RelativePivotInitialLagLocation};
	auto PreviousSubstepTime{0.0f};

	for (auto SubstepNumber{1};; SubstepNumber++)
	{
		const auto SubstepTime{SubstepNumber * Settings->CameraLagSubstepping.LagSubstepDeltaTime};
		if (SubstepTime < DeltaTime - UE_SMALL_NUMBER)
		{
			const auto SubstepRelativePivotTargetLocation{RelativePivotInitialLagLocation + SubstepMovementSpeed * SubstepTime};
			const auto SubstepDeltaTime{SubstepTime - PreviousSubstepTime};

			RelativePivotLagLocation.X = FMath::FInterpTo(RelativePivotLagLocation.X, SubstepRelativePivotTargetLocation.X,
			                                              SubstepDeltaTime, LocationLagX);
			RelativePivotLagLocation.Y = FMath::FInterpTo(RelativePivotLagLocation.Y, SubstepRelativePivotTargetLocation.Y,
			                                              SubstepDeltaTime, LocationLagY);
			RelativePivotLagLocation.Z = FMath::FInterpTo(RelativePivotLagLocation.Z, SubstepRelativePivotTargetLocation.Z,
			                                              SubstepDeltaTime, LocationLagZ);

			PreviousSubstepTime = SubstepTime;
		}
		else
		{
			const auto RemainingDeltaTime{DeltaTime - PreviousSubstepTime};

			RelativePivotLagLocation.X = FMath::FInterpTo(RelativePivotLagLocation.X, RelativePivotTargetLocation.X,
			                                              RemainingDeltaTime, LocationLagX);
			RelativePivotLagLocation.Y = FMath::FInterpTo(RelativePivotLagLocation.Y, RelativePivotTargetLocation.Y,
			                                              RemainingDeltaTime, LocationLagY);
			RelativePivotLagLocation.Z = FMath::FInterpTo(RelativePivotLagLocation.Z, RelativePivotTargetLocation.Z,
			                                              RemainingDeltaTime, LocationLagZ);

			return CameraYawRotation.RotateVector(RelativePivotLagLocation);
		}
	}
}

FVector UAlsCameraComponent::CalculatePivotOffset() const
{
	return Character->GetMesh()->GetComponentQuat().RotateVector(
		FVector{
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetXCurveName()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetYCurveName()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetZCurveName())
		} * Character->GetMesh()->GetComponentScale().Z);
}

FVector UAlsCameraComponent::CalculateCameraOffset() const
{
	return CameraRotation.RotateVector(
		FVector{
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetXCurveName()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetYCurveName()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetZCurveName())
		} * Character->GetMesh()->GetComponentScale().Z);
}

FVector UAlsCameraComponent::CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
                                                  const float DeltaTime, const bool bAllowLag, float& NewTraceDistanceRatio) const
{
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraTraces{
		UAlsUtility::ShouldDisplayDebugForActor(GetOwner(), UAlsCameraConstants::CameraTracesDebugDisplayName())
	};
#else
	const auto bDisplayDebugCameraTraces{false};
#endif

	const auto MeshScale{Character->GetMesh()->GetComponentScale().Z};

	static const FName MainTraceTag{FString::Printf(TEXT("%hs (Main Trace)"), __FUNCTION__)};

	auto TraceStart{
		FMath::Lerp(
			GetThirdPersonTraceStartLocation(),
			PivotTargetLocation + PivotOffset + FVector{Settings->ThirdPerson.TraceOverrideOffset},
			UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::TraceOverrideCurveName())))
	};

	const auto TraceEnd{CameraTargetLocation};

	const auto TraceChanel{UEngineTypes::ConvertToCollisionChannel(Settings->ThirdPerson.TraceChannel)};
	const auto CollisionShape{FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale)};

	auto TraceResult{TraceEnd};

	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, TraceChanel,
	                                     CollisionShape, {MainTraceTag, false, GetOwner()}))
	{
		if (!Hit.bStartPenetrating)
		{
			TraceResult = Hit.Location;
		}
		else if (TryAdjustLocationBlockedByGeometry(TraceStart, bDisplayDebugCameraTraces))
		{
			static const FName AdjustedTraceTag{FString::Printf(TEXT("%hs (Adjusted Trace)"), __FUNCTION__)};

			GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, TraceChanel,
			                                 CollisionShape, {AdjustedTraceTag, false, GetOwner()});
			if (Hit.IsValidBlockingHit())
			{
				TraceResult = Hit.Location;
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		UAlsUtility::DrawDebugSweptSphere(GetWorld(), TraceStart, TraceResult, Settings->ThirdPerson.TraceRadius * MeshScale,
		                                  Hit.IsValidBlockingHit() ? FLinearColor::Red : FLinearColor::Green);
	}
#endif

	// Apply trace distance smoothing.

	if (!bAllowLag || !Settings->ThirdPerson.bEnableTraceDistanceSmoothing)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TraceVector{TraceEnd - TraceStart};
	const auto TraceDistance{TraceVector.Size()};

	if (TraceDistance <= UE_KINDA_SMALL_NUMBER)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TargetTraceDistanceRatio{UE_REAL_TO_FLOAT((TraceResult - TraceStart).Size() / TraceDistance)};

	NewTraceDistanceRatio = TargetTraceDistanceRatio <= TraceDistanceRatio
		                        ? TargetTraceDistanceRatio
		                        : UAlsMath::ExponentialDecay(TraceDistanceRatio, TargetTraceDistanceRatio, DeltaTime,
		                                                     Settings->ThirdPerson.TraceDistanceSmoothing.InterpolationSpeed);

	return TraceStart + TraceVector * TraceDistanceRatio;
}

bool UAlsCameraComponent::TryAdjustLocationBlockedByGeometry(FVector& Location, const bool bDisplayDebugCameraTraces) const
{
	// Based on ComponentEncroachesBlockingGeometry_WithAdjustment().

	const auto MeshScale{Character->GetMesh()->GetComponentScale().Z};

	const auto TraceChanel{UEngineTypes::ConvertToCollisionChannel(Settings->ThirdPerson.TraceChannel)};
	const auto CollisionShape{FCollisionShape::MakeSphere((Settings->ThirdPerson.TraceRadius + 1.0f) * MeshScale)};

	static TArray<FOverlapResult> Overlaps;
	check(Overlaps.IsEmpty())

	ON_SCOPE_EXIT
	{
		Overlaps.Reset();
	};

	static const FName OverlapMultiTraceTag{FString::Printf(TEXT("%hs (Overlap Multi)"), __FUNCTION__)};

	if (!GetWorld()->OverlapMultiByChannel(Overlaps, Location, FQuat::Identity, TraceChanel,
	                                       CollisionShape, {OverlapMultiTraceTag, false, GetOwner()}))
	{
		return false;
	}

	auto Adjustment{FVector::ZeroVector};
	auto bAnyValidBlock{false};

	FMTDResult MtdResult;

	for (const auto& Overlap : Overlaps)
	{
		if (!Overlap.Component.IsValid() || Overlap.Component->GetCollisionResponseToChannel(TraceChanel) != ECR_Block)
		{
			continue;
		}

		const auto* OverlapBodyInstance{Overlap.Component->GetBodyInstance(NAME_None, true, Overlap.ItemIndex)};

		if (OverlapBodyInstance == nullptr ||
		    !OverlapBodyInstance->OverlapTest(Location, FQuat::Identity, CollisionShape, &MtdResult))
		{
			return false;
		}

		if (!FMath::IsNearlyZero(MtdResult.Distance))
		{
			Adjustment += MtdResult.Direction * MtdResult.Distance;
			bAnyValidBlock = true;
		}
	}

	if (!bAnyValidBlock)
	{
		return false;
	}

	auto AdjustmentDirection{Adjustment};

	if (!AdjustmentDirection.Normalize() ||
	    UAlsMath::AngleBetweenSkipNormalization((GetOwner()->GetActorLocation() - Location).GetSafeNormal(),
	                                            AdjustmentDirection) > 90.0f + 1.0f)
	{
		return false;
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		DrawDebugLine(GetWorld(), Location, Location + Adjustment,
		              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
		              false, 5.0f, 0, UAlsUtility::DrawLineThickness);
	}
#endif

	Location += Adjustment;

	static const FName FreeSpaceTraceTag{FString::Printf(TEXT("%hs (Free Space Overlap)"), __FUNCTION__)};

	return !GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, TraceChanel,
	                                                 FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale),
	                                                 {FreeSpaceTraceTag, false, GetOwner()});
}
