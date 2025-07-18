#include "AlsCameraComponent.h"

#include "AlsCameraSettings.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/WorldSettings.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsDebugUtility.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsRotation.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsCameraComponent)

UAlsCameraComponent::UAlsCameraComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = false;
	bHiddenInGame = true;
}

void UAlsCameraComponent::PostLoad()
{
	Super::PostLoad();

	// This is required for the camera to work properly, as its mesh is never rendered.
	// We change the tick option here to override the value that comes from the config file.
	VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
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
	if (bReset || ShouldActivate())
	{
		TickCamera(0.0f, false);
	}

	Super::Activate(bReset);
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
	const auto* Mesh{Character->GetMesh()};

	FVector FirstPivotLocation;

	if (!IsValid(Mesh->GetAttachParent()) && Settings->ThirdPerson.FirstPivotSocketName == UAlsConstants::RootBoneName())
	{
		// The root bone location usually remains fixed when the mesh is detached, so use the capsule's bottom location here as a fallback.

		FirstPivotLocation = Character->GetRootComponent()->GetComponentLocation();
		FirstPivotLocation.Z -= Character->GetRootComponent()->Bounds.BoxExtent.Z;
	}
	else
	{
		FirstPivotLocation = Mesh->GetSocketLocation(Settings->ThirdPerson.FirstPivotSocketName);
	}

	return (FirstPivotLocation + Mesh->GetSocketLocation(Settings->ThirdPerson.SecondPivotSocketName)) * 0.5f;
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
	ViewInfo.FOV = CameraFieldOfView;

	ViewInfo.PostProcessBlendWeight = IsValid(Settings) ? PostProcessWeight : 0.0f;

	if (ViewInfo.PostProcessBlendWeight > UE_SMALL_NUMBER)
	{
		ViewInfo.PostProcessSettings = Settings->PostProcess;
	}
}

void UAlsCameraComponent::TickCamera(const float DeltaTime, bool bAllowLag)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsCameraComponent::TickCamera"), STAT_UAlsCameraComponent_TickCamera, STATGROUP_Als)
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(__FUNCTION__)

	if (!IsValid(GetAnimInstance()) || !IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	ALS_ENSURE_MESSAGE(!IsRunningParallelEvaluation(), // NOLINT(clang-diagnostic-unused-value)
	                   TEXT("UAlsCameraComponent::TickCamera() should not be called during parallel animation")
	                   TEXT(" evaluation, because accessing animation curves causes the game thread to wait")
	                   TEXT(" for the parallel task to complete, resulting in performance degradation"));

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraShapes{
		UAlsDebugUtility::ShouldDisplayDebugForActor(GetOwner(), UAlsCameraConstants::CameraShapesDebugDisplayName())
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

		CameraFieldOfView = bOverrideFieldOfView ? FieldOfViewOverride : Settings->FirstPerson.FieldOfView;
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

	const FQuat CameraYawRotation{FVector::ZAxisVector, FMath::DegreesToRadians(CameraRotation.Yaw)};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UAlsDebugUtility::DrawSphereAlternative(GetWorld(), PivotTargetLocation, CameraYawRotation.Rotator(), 16.0f, FLinearColor::Green);
	}
#endif

	// Calculate pivot lag location. Get the pivot target location and interpolate using axis-independent lag for maximum control.

	if (bMovementBaseHasRelativeRotation)
	{
		PivotLagLocation = MovementBaseLocation + MovementBaseRotation.RotateVector(PivotMovementBaseRelativeLagLocation);

		PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation, DeltaTime, bAllowLag);

		PivotMovementBaseRelativeLagLocation = MovementBaseRotation.UnrotateVector(PivotLagLocation - MovementBaseLocation);
	}
	else
	{
		PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation, DeltaTime, bAllowLag);
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLagLocation, PivotTargetLocation,
		              FLinearColor{1.0f, 0.5f, 0.0f}.ToFColor(true),
		              false, 0.0f, 0, UAlsDebugUtility::DrawLineThickness);

		UAlsDebugUtility::DrawSphereAlternative(GetWorld(), PivotLagLocation, CameraYawRotation.Rotator(), 16.0f, {1.0f, 0.5f, 0.0f});
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
		              false, 0.0f, 0, UAlsDebugUtility::DrawLineThickness);

		UAlsDebugUtility::DrawSphereAlternative(GetWorld(), PivotLocation, CameraYawRotation.Rotator(), 16.0f, {0.0f, 0.75f, 1.0f});
	}
#endif

	// Calculate target camera location.

	const auto CameraTargetLocation{PivotLocation + CalculateCameraOffset()};

	// Trace for an object between the camera and character to apply a corrective offset.

	const auto CameraFinalLocation{CalculateCameraTrace(CameraTargetLocation, PivotOffset, DeltaTime, bAllowLag, TraceDistanceRatio)};

	if (!FAnimWeight::IsRelevant(FirstPersonOverride))
	{
		CameraLocation = CameraFinalLocation;
		CameraFieldOfView = Settings->ThirdPerson.FieldOfView;
	}
	else
	{
		CameraLocation = FMath::Lerp(CameraFinalLocation, GetFirstPersonCameraLocation(), FirstPersonOverride);
		CameraFieldOfView = FMath::Lerp(Settings->ThirdPerson.FieldOfView, Settings->FirstPerson.FieldOfView, FirstPersonOverride);
	}

	if (bOverrideFieldOfView)
	{
		CameraFieldOfView = FieldOfViewOverride;
	}

	CameraFieldOfView = FMath::Clamp(CameraFieldOfView + CalculateFovOffset(), 5.0f, 175.0f);
}

FRotator UAlsCameraComponent::CalculateCameraRotation(const FRotator& CameraTargetRotation,
                                                      const float DeltaTime, const bool bAllowLag) const
{
	if (!bAllowLag)
	{
		return CameraTargetRotation;
	}

	const auto RotationLag{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::RotationLagCurveName())};

	return UAlsRotation::ExponentialDecayRotation(CameraRotation, CameraTargetRotation, DeltaTime, RotationLag);
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

	return CameraYawRotation.RotateVector({
		UAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.X, RelativePivotTargetLocation.X, DeltaTime, LocationLagX),
		UAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.Y, RelativePivotTargetLocation.Y, DeltaTime, LocationLagY),
		UAlsMath::ExponentialDecay(RelativePivotInitialLagLocation.Z, RelativePivotTargetLocation.Z, DeltaTime, LocationLagZ)
	});
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

float UAlsCameraComponent::CalculateFovOffset() const
{
	return GetAnimInstance()->GetCurveValue(UAlsCameraConstants::FovOffsetCurveName());
}

FVector UAlsCameraComponent::CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
                                                  const float DeltaTime, const bool bAllowLag, float& NewTraceDistanceRatio) const
{
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraTraces{
		UAlsDebugUtility::ShouldDisplayDebugForActor(GetOwner(), UAlsCameraConstants::CameraTracesDebugDisplayName())
	};
#else
	const auto bDisplayDebugCameraTraces{false};
#endif

	const auto MeshScale{UE_REAL_TO_FLOAT(Character->GetMesh()->GetComponentScale().Z)};

	static const FName MainTraceTag{FString::Printf(TEXT("%hs (Main Trace)"), __FUNCTION__)};

	auto TraceStart{
		FMath::Lerp(
			GetThirdPersonTraceStartLocation(),
			PivotTargetLocation + PivotOffset + FVector{Settings->ThirdPerson.TraceOverrideOffset},
			UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::TraceOverrideCurveName())))
	};

	const auto TraceEnd{CameraTargetLocation};
	const auto CollisionShape{FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale)};

	auto TraceResult{TraceEnd};

	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                     CollisionShape, {MainTraceTag, false, GetOwner()}))
	{
		if (!Hit.bStartPenetrating)
		{
			TraceResult = Hit.Location;
		}
		else if (TryAdjustLocationBlockedByGeometry(TraceStart, bDisplayDebugCameraTraces))
		{
			static const FName AdjustedTraceTag{FString::Printf(TEXT("%hs (Adjusted Trace)"), __FUNCTION__)};

			GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
			                                 CollisionShape, {AdjustedTraceTag, false, GetOwner()});
			if (Hit.IsValidBlockingHit())
			{
				TraceResult = Hit.Location;
			}
		}
		else
		{
			// Note that TraceStart may be changed even if TryAdjustLocationBlockedByGeometry() returned false.
			TraceResult = TraceStart;
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		UAlsDebugUtility::DrawSweepSphere(GetWorld(), TraceStart, TraceResult, CollisionShape.GetCapsuleRadius(),
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

	const auto MeshScale{UE_REAL_TO_FLOAT(Character->GetMesh()->GetComponentScale().Z)};
	const auto CollisionShape{FCollisionShape::MakeSphere((Settings->ThirdPerson.TraceRadius + 1.0f) * MeshScale)};

	static TArray<FOverlapResult> Overlaps;
	check(Overlaps.IsEmpty())

	ON_SCOPE_EXIT
	{
		Overlaps.Reset();
	};

	static const FName OverlapMultiTraceTag{FString::Printf(TEXT("%hs (Overlap Multi)"), __FUNCTION__)};

	if (!GetWorld()->OverlapMultiByChannel(Overlaps, Location, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                       CollisionShape, {OverlapMultiTraceTag, false, GetOwner()}))
	{
		return false;
	}

	auto Adjustment{FVector::ZeroVector};
	auto bAnyValidBlock{false};

	FMTDResult MtdResult;

	for (const auto& Overlap : Overlaps)
	{
		if (!Overlap.Component.IsValid() ||
		    Overlap.Component->GetCollisionResponseToChannel(Settings->ThirdPerson.TraceChannel) != ECR_Block)
		{
			continue;
		}

		const auto* OverlapBody{Overlap.Component->GetBodyInstance(NAME_None, true, Overlap.ItemIndex)};

		if (OverlapBody == nullptr || !OverlapBody->OverlapTest(Location, FQuat::Identity, CollisionShape, &MtdResult))
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
	    ((GetOwner()->GetActorLocation() - Location).GetSafeNormal() | AdjustmentDirection) < -UE_KINDA_SMALL_NUMBER)
	{
		return false;
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		DrawDebugLine(GetWorld(), Location, Location + Adjustment,
		              FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true),
		              false, 5.0f, 0, UAlsDebugUtility::DrawLineThickness);
	}
#endif

	Location += Adjustment;

	static const FName FreeSpaceTraceTag{FString::Printf(TEXT("%hs (Free Space Overlap)"), __FUNCTION__)};

	return !GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, Settings->ThirdPerson.TraceChannel,
	                                                 FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * MeshScale),
	                                                 {FreeSpaceTraceTag, false, GetOwner()});
}
