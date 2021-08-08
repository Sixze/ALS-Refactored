#include "AlsCameraComponent.h"

#include "AlsCameraAnimationInstance.h"
#include "AlsCharacter.h"
#include "DisplayDebugHelpers.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraTypes.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

UAlsCameraComponent::UAlsCameraComponent()
{
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	bTickInEditor = false;
	bHiddenInGame = true;

	AnimClass = UAlsCameraAnimationInstance::StaticClass();
}

void UAlsCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	check(IsValid(SkeletalMesh))

	AlsCharacter = Cast<AAlsCharacter>(GetOwner());
	check(IsValid(AlsCharacter))

	TickCamera(0.0f, false);
}

void UAlsCameraComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCamera(DeltaTime);
}

FVector UAlsCameraComponent::GetFirstPersonPivotLocation() const
{
	return AlsCharacter->GetMesh()->GetSocketLocation(FirstPersonSocket);
}

FTransform UAlsCameraComponent::GetThirdPersonPivotTransform() const
{
	return {
		GetComponentRotation(),
		(AlsCharacter->GetMesh()->GetSocketLocation(ThirdPersonFirstSocket) +
		 AlsCharacter->GetMesh()->GetSocketLocation(ThirdPersonSecondSocket)) * 0.5f
	};
}

void UAlsCameraComponent::GetViewInfo(FMinimalViewInfo& ViewInfo) const
{
	ViewInfo.Location = CameraLocation;
	ViewInfo.Rotation = CameraRotation;
	ViewInfo.FOV = CameraFov;
}

void UAlsCameraComponent::TickCamera(float DeltaTime, bool bAllowLag)
{
#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebugCameraShapes{UAlsUtility::ShouldDisplayDebug(GetOwner(), UAlsCameraConstants::CameraShapesDisplayName())};
	const auto bDisplayDebugCameraTraces{UAlsUtility::ShouldDisplayDebug(GetOwner(), UAlsCameraConstants::CameraTracesDisplayName())};
#endif

	// Calculate result rotation. Use the view rotation and interpolate for smooth rotation.

	FQuat ResultRotation;

	if (bAllowLag)
	{
		ResultRotation = UAlsMath::ExponentialDecay(CameraRotation.Quaternion(), AlsCharacter->GetViewState().SmoothRotation.Quaternion(),
		                                            GetAnimInstance()->GetCurveValue(UAlsCameraConstants::RotationLagCurve()), DeltaTime);
	}
	else
	{
		ResultRotation = AlsCharacter->GetViewState().SmoothRotation.Quaternion();
	}

	CameraRotation = ResultRotation.Rotator();

	const auto FirstPersonAmount{UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::FirstPersonAmountCurve()))};
	if (FirstPersonAmount >= 1.0f)
	{
		PivotTargetLocation = GetThirdPersonPivotTransform().GetLocation();
		PivotLagLocation = PivotTargetLocation;
		PivotLocation = PivotTargetLocation;

		CameraLocation = GetFirstPersonPivotLocation();
		CameraFov = FirstPersonFov;
		return;
	}

	// Calculate pivot lag location. Get the target pivot transform and
	// interpolate using axis independent lag for maximum control.

	const auto PivotTargetTransform{GetThirdPersonPivotTransform()};

	PivotTargetLocation = PivotTargetTransform.GetLocation();

	const auto LagRotation{FRotator{0.0f, CameraRotation.Yaw, 0.0f}.Quaternion()};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotTargetLocation, LagRotation.Rotator(), 16.0f, FLinearColor::Green);
	}
#endif

	if (bAllowLag)
	{
		const auto RelativePreviousPivotLagLocation{LagRotation.UnrotateVector(PivotLagLocation)};
		const auto RelativeTargetPivotLocation{LagRotation.UnrotateVector(PivotTargetLocation)};

		PivotLagLocation = LagRotation.RotateVector({
			UAlsMath::ExponentialDecay(RelativePreviousPivotLagLocation.X, RelativeTargetPivotLocation.X,
			                           GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagXCurve()), DeltaTime),
			UAlsMath::ExponentialDecay(RelativePreviousPivotLagLocation.Y, RelativeTargetPivotLocation.Y,
			                           GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagYCurve()), DeltaTime),
			UAlsMath::ExponentialDecay(RelativePreviousPivotLagLocation.Z, RelativeTargetPivotLocation.Z,
			                           GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagZCurve()), DeltaTime)
		});
	}
	else
	{
		PivotLagLocation = PivotTargetLocation;
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLagLocation, PivotTargetLocation,
		              FLinearColor{1.0f, 0.5f, 0.0f}.ToFColor(true), false, 0.0f, 0, 1.0f);

		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotLagLocation, LagRotation.Rotator(), 16.0f, {1.0f, 0.5f, 0.0f});
	}
#endif

	// Calculate pivot location. Get the pivot lag location and apply local offsets for further camera control.

	PivotLocation = PivotLagLocation +
	                PivotTargetTransform.TransformVectorNoScale({
		                GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetXCurve()),
		                GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetYCurve()),
		                GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetZCurve())
	                });

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraShapes)
	{
		DrawDebugLine(GetWorld(), PivotLocation, PivotLagLocation, FLinearColor{0.0f, 0.75f, 1.0f}.ToFColor(true), false, 0.0f, 0, 1.0f);

		UAlsUtility::DrawDebugSphereAlternative(GetWorld(), PivotLocation, LagRotation.Rotator(), 16.0f, {0.0f, 0.75f, 1.0f});
	}
#endif

	// Calculate result location. Get the pivot location and apply camera relative offsets.

	auto ResultLocation{
		PivotLocation + ResultRotation.RotateVector({
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetXCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetYCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetZCurve())
		})
	};

	// Trace for an object between the camera and character to apply a corrective offset.

	auto TraceStart{
		AlsCharacter->GetMesh()->GetSocketLocation(bRightShoulder ? ThirdPersonTraceRightSocket : ThirdPersonTraceLeftSocket)
	};

	FHitResult Hit;
	GetWorld()->SweepSingleByChannel(Hit, TraceStart, ResultLocation, FQuat::Identity,
	                                 UEngineTypes::ConvertToCollisionChannel(ThirdPersonTraceChannel),
	                                 FCollisionShape::MakeSphere(ThirdPersonTraceRadius),
	                                 {__FUNCTION__, false, GetOwner()});

	if (Hit.bBlockingHit)
	{
		if (!Hit.bStartPenetrating)
		{
			ResultLocation = Hit.Location;
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebugCameraTraces)
	{
		UAlsUtility::DrawDebugSweptSphere(GetWorld(), TraceStart, ResultLocation, ThirdPersonTraceRadius,
		                                  Hit.IsValidBlockingHit() ? FLinearColor::Red : FLinearColor::Green);
	}
#endif

	if (FirstPersonAmount <= 0.0f)
	{
		CameraLocation = ResultLocation;
		CameraFov = ThirdPersonFov;
	}
	else
	{
		CameraLocation = FMath::Lerp(ResultLocation, GetFirstPersonPivotLocation(), FirstPersonAmount);
		CameraFov = FMath::Lerp(ThirdPersonFov, FirstPersonFov, FirstPersonAmount);
	}
}

void UAlsCameraComponent::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& VerticalPosition) const
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

void UAlsCameraComponent::DisplayDebugHeader(UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
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

void UAlsCameraComponent::DisplayDebugCurves(UCanvas* Canvas, const float Scale,
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

	TArray<FName> CurveNames;
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
}

void UAlsCameraComponent::DisplayDebugShapes(UCanvas* Canvas, const float Scale,
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

void UAlsCameraComponent::DisplayDebugTraces(UCanvas* Canvas, const float Scale,
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

	static const auto CameraTraceNoHitText{FText::AsCultureInvariant(TEXT("Camera Trace (No Hit)"))};

	Text.SetColor(FLinearColor::Green);

	Text.Text = CameraTraceNoHitText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto CameraTraceHitText{FText::AsCultureInvariant(TEXT("Camera Trace (Object Hit)"))};

	Text.SetColor(FLinearColor::Red);

	Text.Text = CameraTraceHitText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;
}
