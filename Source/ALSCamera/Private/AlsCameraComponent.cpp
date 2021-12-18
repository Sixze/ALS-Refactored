#include "AlsCameraComponent.h"

#include "AlsCameraAnimationInstance.h"
#include "AlsCharacter.h"
#include "DisplayDebugHelpers.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraTypes.h"
#include "Engine/Canvas.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsUtility.h"

UAlsCameraComponent::UAlsCameraComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = false;
	bHiddenInGame = true;

	AnimClass = UAlsCameraAnimationInstance::StaticClass();
}

#if WITH_EDITORONLY_DATA
void UAlsCameraComponent::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);

	if (Archive.IsLoading())
	{
		PostProcessSettings.OnAfterLoad();
	}
}
#endif

void UAlsCameraComponent::OnRegister()
{
	AlsCharacter = Cast<AAlsCharacter>(GetOwner());

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
	Super::BeginPlay();

	ensure(IsValid(SkeletalMesh) && IsValid(AlsCharacter));
}

void UAlsCameraComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCamera(DeltaTime);
}

FVector UAlsCameraComponent::GetFirstPersonCameraLocation() const
{
	return AlsCharacter->GetMesh()->GetSocketLocation(FirstPersonSettings.CameraSocket);
}

FTransform UAlsCameraComponent::GetThirdPersonPivotTransform() const
{
	return {
		GetComponentRotation(),
		(AlsCharacter->GetMesh()->GetSocketLocation(ThirdPersonSettings.FirstPivotSocket) +
		 AlsCharacter->GetMesh()->GetSocketLocation(ThirdPersonSettings.SecondPivotSocket)) * 0.5f
	};
}

FVector UAlsCameraComponent::GetThirdPersonTraceStartLocation() const
{
	return AlsCharacter->GetMesh()->GetSocketLocation(bRightShoulder
		                                                  ? ThirdPersonSettings.RightTraceShoulderSocket
		                                                  : ThirdPersonSettings.LeftTraceShoulderSocket);
}

void UAlsCameraComponent::GetViewInfo(FMinimalViewInfo& ViewInfo) const
{
	ViewInfo.Location = CameraLocation;
	ViewInfo.Rotation = CameraRotation;
	ViewInfo.FOV = CameraFov;

	ViewInfo.PostProcessBlendWeight = PostProcessWeight;
	if (PostProcessWeight > 0.0f)
	{
		ViewInfo.PostProcessSettings = PostProcessSettings;
	}
}

void UAlsCameraComponent::TickCamera(const float DeltaTime, const bool bAllowLag)
{
	if (!IsValid(AlsCharacter) || !IsValid(GetAnimInstance()))
	{
		return;
	}

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraShapes{UAlsUtility::ShouldDisplayDebug(GetOwner(), UAlsCameraConstants::CameraShapesDisplayName())};
#else
	const auto bDisplayDebugCameraShapes{false};
#endif

	// Calculate camera rotation. Use raw rotation locally and smooth rotation on remote clients.

	const auto CameraTargetRotation{
		AlsCharacter->IsLocallyControlled()
			? AlsCharacter->GetViewRotation()
			: AlsCharacter->GetViewState().SmoothRotation
	};

	CameraRotation = CalculateCameraRotation(CameraTargetRotation, DeltaTime, bAllowLag);
	CameraRotation.Normalize();

	const auto PivotTargetTransform{GetThirdPersonPivotTransform()};

	PivotTargetLocation = PivotTargetTransform.GetLocation();

	const auto FirstPersonOverride{UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::FirstPersonOverrideCurve()))};
	if (FirstPersonOverride >= 1.0f)
	{
		PivotLagLocation = PivotTargetLocation;
		PivotLocation = PivotTargetLocation;

		CameraLocation = GetFirstPersonCameraLocation();
		CameraFov = FirstPersonSettings.Fov;
		return;
	}

	const FRotator CameraYawRotation{0.0f, CameraRotation.Yaw, 0.0f};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotTargetLocation, CameraYawRotation, 16.0f, FLinearColor::Green);
	}
#endif

	// Calculate pivot lag location. Get the pivot target location and interpolate using axis independent lag for maximum control.

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

	if (FirstPersonOverride <= 0.0f)
	{
		CameraLocation = CameraResultLocation;
		CameraFov = ThirdPersonSettings.Fov;
	}
	else
	{
		CameraLocation = FMath::Lerp(CameraResultLocation, GetFirstPersonCameraLocation(), FirstPersonOverride);
		CameraFov = FMath::Lerp(ThirdPersonSettings.Fov, FirstPersonSettings.Fov, FirstPersonOverride);
	}
}

FRotator UAlsCameraComponent::CalculateCameraRotation(const FRotator& CameraTargetRotation, const float DeltaTime,
                                                      const bool bAllowLag) const
{
	if (!bAllowLag)
	{
		return CameraTargetRotation;
	}

	const auto RotationLag{GetAnimInstance()->GetCurveValue(UAlsCameraConstants::RotationLagCurve())};

	if (!bUseLagSubstepping || DeltaTime <= LagSubstepDeltaTime || RotationLag <= 0.0f)
	{
		return UAlsMath::ExponentialDecay(CameraRotation, CameraTargetRotation, DeltaTime, RotationLag);
	}

	const auto CameraInitialRotation{CameraRotation};
	const auto SubstepRotationSpeed{(CameraTargetRotation - CameraInitialRotation).GetNormalized() * (1.0f / DeltaTime)};

	auto NewCameraRotation{CameraRotation};
	auto PreviousSubstepTime{0.0f};

	for (auto SubstepNumber{1};; SubstepNumber++)
	{
		const auto SubstepTime{SubstepNumber * LagSubstepDeltaTime};
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
	if (!bUseLagSubstepping || DeltaTime <= LagSubstepDeltaTime || (LocationLagX <= 0.0f && LocationLagY <= 0.0f && LocationLagZ <= 0.0f))
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
		const auto SubstepTime{SubstepNumber * LagSubstepDeltaTime};
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
	return PivotTargetRotation.RotateVector({
		GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetXCurve()),
		GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetYCurve()),
		GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetZCurve())
	});
}

FVector UAlsCameraComponent::CalculateCameraOffset() const
{
	return CameraRotation.RotateVector({
		GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetXCurve()),
		GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetYCurve()),
		GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetZCurve())
	});
}

FVector UAlsCameraComponent::CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
                                                  const float DeltaTime, const bool bAllowLag, float& NewTraceDistanceRatio) const
{
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraTraces{UAlsUtility::ShouldDisplayDebug(GetOwner(), UAlsCameraConstants::CameraTracesDisplayName())};
#else
	const auto bDisplayDebugCameraTraces{false};
#endif

	static const FName MainTraceTag{FString::Format(TEXT("{0} (Main Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	auto TraceStart{
		FMath::Lerp(
			GetThirdPersonTraceStartLocation(),
			PivotTargetLocation + PivotOffset + ThirdPersonSettings.TraceOverrideOffset,
			UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::TraceOverrideCurve())))
	};

	const auto TraceEnd{CameraTargetLocation};

	const auto TraceChanel{UEngineTypes::ConvertToCollisionChannel(ThirdPersonSettings.TraceChannel)};
	const auto CollisionShape{FCollisionShape::MakeSphere(ThirdPersonSettings.TraceRadius)};

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
		UAlsUtility::DrawDebugSweptSphere(GetWorld(), TraceStart, TraceResult, ThirdPersonSettings.TraceRadius,
		                                  Hit.IsValidBlockingHit() ? FLinearColor::Red : FLinearColor::Green);
	}
#endif

	// Apply trace distance smoothing.

	if (!ThirdPersonSettings.bAllowTraceDistanceSmoothing || !bAllowLag)
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

	const auto TargetTraceDistanceRatio{(TraceResult - TraceStart).Size() / TraceDistance};

	NewTraceDistanceRatio = TargetTraceDistanceRatio <= TraceDistanceRatio
		                        ? TargetTraceDistanceRatio
		                        : UAlsMath::ExponentialDecay(TraceDistanceRatio, TargetTraceDistanceRatio, DeltaTime,
		                                                     ThirdPersonSettings.TraceDistanceInterpolationSpeed);

	return TraceStart + TraceVector * TraceDistanceRatio;
}

bool UAlsCameraComponent::TryFindBlockingGeometryAdjustedLocation(FVector& Location, const bool bDisplayDebugCameraTraces) const
{
	// Copied with modifications from ComponentEncroachesBlockingGeometry_WithAdjustment

	constexpr auto Epsilon{1.0f};

	const auto TraceChanel{UEngineTypes::ConvertToCollisionChannel(ThirdPersonSettings.TraceChannel)};
	const auto CollisionShape{FCollisionShape::MakeSphere(ThirdPersonSettings.TraceRadius + Epsilon)};

	static TArray<FOverlapResult> Overlaps;
	check(Overlaps.Num() <= 0)

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

		if (!Overlap.Component->ComputePenetration(MtdResult, CollisionShape, Location, FQuat::Identity))
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
	if (!AdjustmentDirection.Normalize())
	{
		return false;
	}

	if (UAlsMath::AngleBetweenSkipNormalization((GetOwner()->GetActorLocation() - Location).GetSafeNormal(),
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
	                                                 FCollisionShape::MakeSphere(ThirdPersonSettings.TraceRadius),
	                                                 {FreeSpaceTraceTag, false, GetOwner()});
}

void UAlsCameraComponent::DisplayDebug(const UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& VerticalPosition) const
{
	const auto Scale{FMath::Min(Canvas->SizeX / (1280.0f * Canvas->GetDPIScale()), Canvas->SizeY / (720.0f * Canvas->GetDPIScale()))};

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{200.0f * Scale};

	auto MaxVerticalPosition{VerticalPosition};
	auto HorizontalPosition{5.0f * Scale};

	if (!DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraCurvesDisplayName()) &&
	    !DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraShapesDisplayName()) &&
	    !DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraTracesDisplayName()))
	{
		VerticalPosition = MaxVerticalPosition;
		return;
	}

	const auto InitialVerticalPosition{VerticalPosition};

	static const auto CameraCurvesHeaderText{FText::AsCultureInvariant(TEXT("ALS.CameraCurves (Shift + 6)"))};

	if (DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraCurvesDisplayName()))
	{
		DisplayDebugHeader(Canvas, CameraCurvesHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugCurves(Canvas, Scale, HorizontalPosition, VerticalPosition);

		MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition + RowOffset);
		VerticalPosition = InitialVerticalPosition;
		HorizontalPosition += ColumnOffset;
	}
	else
	{
		DisplayDebugHeader(Canvas, CameraCurvesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);

		VerticalPosition += RowOffset;
	}

	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	static const auto ShapesHeaderText{FText::AsCultureInvariant(TEXT("ALS.CameraShapes (Shift + 7)"))};

	if (DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraShapesDisplayName()))
	{
		DisplayDebugHeader(Canvas, ShapesHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugShapes(Canvas, Scale, HorizontalPosition, VerticalPosition);
	}
	else
	{
		DisplayDebugHeader(Canvas, ShapesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);
	}

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	static const auto TracesHeaderText{FText::AsCultureInvariant(TEXT("ALS.CameraTraces (Shift + 8)"))};

	if (DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraTracesDisplayName()))
	{
		DisplayDebugHeader(Canvas, TracesHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugTraces(Canvas, Scale, HorizontalPosition, VerticalPosition);
	}
	else
	{
		DisplayDebugHeader(Canvas, TracesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);
	}

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	VerticalPosition = MaxVerticalPosition;
}

void UAlsCameraComponent::DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
                                             const float Scale, const float HorizontalPosition, float& VerticalPosition)
{
	FCanvasTextItem Text{
		{HorizontalPosition, VerticalPosition},
		HeaderText,
		GEngine->GetMediumFont(),
		HeaderColor
	};

	Text.Scale = {Scale, Scale};
	Text.EnableShadow(FLinearColor::Black);

	Text.Draw(Canvas->Canvas);

	VerticalPosition += 15.0f * Scale;
}

void UAlsCameraComponent::DisplayDebugCurves(const UCanvas* Canvas, const float Scale,
                                             const float HorizontalPosition, float& VerticalPosition) const
{
	VerticalPosition += 4.0f * Scale;

	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{145.0f * Scale};

	static TArray<FName> CurveNames;
	check(CurveNames.Num() <= 0)

	GetAnimInstance()->GetAllCurveNames(CurveNames);

	CurveNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	for (const auto& CurveName : CurveNames)
	{
		const auto CurveValue{GetAnimInstance()->GetCurveValue(CurveName)};

		Text.SetColor(FMath::Lerp(FLinearColor::Gray, FLinearColor::White, UAlsMath::Clamp01(FMath::Abs(CurveValue))));

		Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(CurveName.ToString(), false));
		Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

		Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), CurveValue));
		Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

		VerticalPosition += RowOffset;
	}

	CurveNames.Reset();
}

void UAlsCameraComponent::DisplayDebugShapes(const UCanvas* Canvas, const float Scale,
                                             const float HorizontalPosition, float& VerticalPosition) const
{
	VerticalPosition += 4.0f * Scale;

	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{120.0f * Scale};

	static const auto PivotTargetLocationText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PivotTargetLocation), false))
	};

	auto Color{FLinearColor::Green};
	Text.SetColor(Color);

	Text.Text = PivotTargetLocationText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      PivotTargetLocation.X, PivotTargetLocation.Y, PivotTargetLocation.Z));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto PivotLagLocationText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PivotLagLocation), false))
	};

	Color = {1.0f, 0.5f, 0.0f};
	Text.SetColor(Color);

	Text.Text = PivotLagLocationText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      PivotLagLocation.X, PivotLagLocation.Y, PivotLagLocation.Z));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto PivotLocationText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PivotLocation), false))
	};

	Color = {0.0f, 0.75f, 1.0f};
	Text.SetColor(Color);

	Text.Text = PivotLocationText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      PivotLocation.X, PivotLocation.Y, PivotLocation.Z));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto CameraFovText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, CameraFov), false))
	};

	Color = FLinearColor::White;
	Text.SetColor(Color);

	Text.Text = CameraFovText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), CameraFov));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto RightShoulderText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, bRightShoulder), true))
	};

	Text.Text = RightShoulderText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(ToString(bRightShoulder), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;
}

void UAlsCameraComponent::DisplayDebugTraces(const UCanvas* Canvas, const float Scale,
                                             const float HorizontalPosition, float& VerticalPosition) const
{
	VerticalPosition += 4.0f * Scale;

	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};

	static const auto BlockingGeometryAdjustmentText{FText::AsCultureInvariant(TEXT("Blocking Geometry Adjustment"))};

	Text.SetColor({0.0f, 0.75f, 1.0f});

	Text.Text = BlockingGeometryAdjustmentText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto CameraTraceNoHitText{FText::AsCultureInvariant(TEXT("Camera Trace (No Hit)"))};

	Text.SetColor(FLinearColor::Green);

	Text.Text = CameraTraceNoHitText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto CameraTraceBlockingHitText{FText::AsCultureInvariant(TEXT("Camera Trace (Blocking Hit)"))};

	Text.SetColor(FLinearColor::Red);

	Text.Text = CameraTraceBlockingHitText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;
}
