#include "AlsCameraComponent.h"

#include "AlsCameraSettings.h"
#include "AlsCharacter.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"

UAlsCameraComponent::UAlsCameraComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = false;
	bHiddenInGame = true;
}

void UAlsCameraComponent::OnRegister()
{
	Character = Cast<AAlsCharacter>(GetOwner());

	Super::OnRegister();
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

void UAlsCameraComponent::BeginPlay()
{
	ALS_ENSURE(IsValid(GetAnimInstance()));
	ALS_ENSURE(IsValid(Settings));
	ALS_ENSURE(IsValid(Character));

	Super::BeginPlay();
}

void UAlsCameraComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCamera(DeltaTime);
}

FVector UAlsCameraComponent::GetFirstPersonCameraLocation() const
{
	return Character->GetMesh()->GetSocketLocation(Settings->FirstPerson.CameraSocketName);
}

FTransform UAlsCameraComponent::GetThirdPersonPivotTransform() const
{
	return {
		GetComponentRotation(),
		(Character->GetMesh()->GetSocketLocation(Settings->ThirdPerson.FirstPivotSocketName) +
		 Character->GetMesh()->GetSocketLocation(Settings->ThirdPerson.SecondPivotSocketName)) * 0.5f
	};
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

	if (ViewInfo.PostProcessBlendWeight > SMALL_NUMBER)
	{
		ViewInfo.PostProcessSettings = Settings->PostProcess;
	}
}

void UAlsCameraComponent::TickCamera(const float DeltaTime, const bool bAllowLag)
{
	if (!IsValid(GetAnimInstance()) || !IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraShapes{UAlsUtility::ShouldDisplayDebug(GetOwner(), UAlsCameraConstants::CameraShapesDisplayName())};
#else
	const auto bDisplayDebugCameraShapes{false};
#endif

	// Calculate camera rotation.

	const auto CameraTargetRotation{
		Character->GetViewState().NetworkSmoothing.bEnabled || Character->IsLocallyControlled()
			? Character->GetViewState().NetworkSmoothing.Rotation
			: Character->GetViewState().Rotation
	};

	CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);

	const auto PivotTargetTransform{GetThirdPersonPivotTransform()};

	PivotTargetLocation = PivotTargetTransform.GetLocation();

	const auto FirstPersonOverride{UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::FirstPersonOverrideCurve()))};
	if (FAnimWeight::IsFullWeight(FirstPersonOverride))
	{
		PivotLagLocation = PivotTargetLocation;
		PivotLocation = PivotTargetLocation;

		CameraLocation = GetFirstPersonCameraLocation();
		CameraFov = Settings->FirstPerson.Fov;
		return;
	}

	const FRotator CameraYawRotation{0.0f, CameraRotation.Yaw, 0.0f};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotTargetLocation, CameraYawRotation, 16.0f, FLinearColor::Green);
	}
#endif

	// Calculate pivot lag location. Get the pivot target location and interpolate using axis-independent lag for maximum control.

	PivotLagLocation = CalculatePivotLagLocation(CameraYawRotation.Quaternion(), DeltaTime, bAllowLag);

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

	const auto PivotOffset{CalculatePivotOffset(PivotTargetTransform.GetRotation())};

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

	const auto RotationLag{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::RotationLagCurve())};

	if (!Settings->bUseLagSubstepping ||
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
		if (SubstepTime < DeltaTime - SMALL_NUMBER)
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

	const auto LocationLagX{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagXCurve())};
	const auto LocationLagY{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagYCurve())};
	const auto LocationLagZ{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagZCurve())};

	// ReSharper disable once CppRedundantParentheses
	if (!Settings->bUseLagSubstepping ||
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
		if (SubstepTime < DeltaTime - SMALL_NUMBER)
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

FVector UAlsCameraComponent::CalculatePivotOffset(const FQuat& PivotTargetRotation) const
{
	return PivotTargetRotation.RotateVector(
		FVector{
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetXCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetYCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetZCurve())
		} * Character->GetCapsuleComponent()->GetComponentScale().Z);
}

FVector UAlsCameraComponent::CalculateCameraOffset() const
{
	return CameraRotation.RotateVector(
		FVector{
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetXCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetYCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetZCurve())
		} * Character->GetCapsuleComponent()->GetComponentScale().Z);
}

FVector UAlsCameraComponent::CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
                                                  const float DeltaTime, const bool bAllowLag, float& NewTraceDistanceRatio) const
{
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraTraces{UAlsUtility::ShouldDisplayDebug(GetOwner(), UAlsCameraConstants::CameraTracesDisplayName())};
#else
	const auto bDisplayDebugCameraTraces{false};
#endif

	const auto CapsuleScale{Character->GetCapsuleComponent()->GetComponentScale().Z};

	static const FName MainTraceTag{FString::Format(TEXT("{0} (Main Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	auto TraceStart{
		FMath::Lerp(
			GetThirdPersonTraceStartLocation(),
			PivotTargetLocation + PivotOffset + Settings->ThirdPerson.TraceOverrideOffset,
			UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::TraceOverrideCurve())))
	};

	const auto TraceEnd{CameraTargetLocation};

	const auto TraceChanel{UEngineTypes::ConvertToCollisionChannel(Settings->ThirdPerson.TraceChannel)};
	const auto CollisionShape{FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * CapsuleScale)};

	auto TraceResult{TraceEnd};

	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, TraceChanel,
	                                     CollisionShape, {MainTraceTag, false, GetOwner()}))
	{
		if (!Hit.bStartPenetrating)
		{
			TraceResult = Hit.Location;
		}
		else if (TryFindBlockingGeometryAdjustedLocation(TraceStart, bDisplayDebugCameraTraces))
		{
			static const FName AdjustedTraceTag{FString::Format(TEXT("{0} (Adjusted Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

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
		UAlsUtility::DrawDebugSweptSphere(GetWorld(), TraceStart, TraceResult, Settings->ThirdPerson.TraceRadius * CapsuleScale,
		                                  Hit.IsValidBlockingHit() ? FLinearColor::Red : FLinearColor::Green);
	}
#endif

	// Apply trace distance smoothing.

	if (!bAllowLag || !Settings->ThirdPerson.bUseTraceDistanceSmoothing)
	{
		NewTraceDistanceRatio = 1.0f;
		return TraceResult;
	}

	const auto TraceVector{TraceEnd - TraceStart};
	const auto TraceDistance{TraceVector.Size()};

	if (TraceDistance <= KINDA_SMALL_NUMBER)
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

bool UAlsCameraComponent::TryFindBlockingGeometryAdjustedLocation(FVector& Location, const bool bDisplayDebugCameraTraces) const
{
	// Based on ComponentEncroachesBlockingGeometry_WithAdjustment().

	const auto CapsuleScale{Character->GetCapsuleComponent()->GetComponentScale().Z};

	const auto TraceChanel{UEngineTypes::ConvertToCollisionChannel(Settings->ThirdPerson.TraceChannel)};
	const auto CollisionShape{FCollisionShape::MakeSphere((Settings->ThirdPerson.TraceRadius + 1.0f) * CapsuleScale)};

	static TArray<FOverlapResult> Overlaps;
	check(Overlaps.IsEmpty())

	static const FName OverlapMultiTraceTag{FString::Format(TEXT("{0} (Overlap Multi)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

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
			Overlaps.Reset();
			return false;
		}

		if (!FMath::IsNearlyZero(MtdResult.Distance))
		{
			Adjustment += MtdResult.Direction * MtdResult.Distance;
			bAnyValidBlock = true;
		}
	}

	Overlaps.Reset();

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

	static const FName FreeSpaceTraceTag{FString::Format(TEXT("{0} (Free Space Overlap)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	return !GetWorld()->OverlapBlockingTestByChannel(Location, FQuat::Identity, TraceChanel,
	                                                 FCollisionShape::MakeSphere(Settings->ThirdPerson.TraceRadius * CapsuleScale),
	                                                 {FreeSpaceTraceTag, false, GetOwner()});
}
