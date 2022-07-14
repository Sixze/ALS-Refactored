#include "AlsCameraComponent.h"

#include "DisplayDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Engine/Canvas.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsUtility.h"

void UAlsCameraComponent::DisplayDebug(const UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& VerticalLocation) const
{
	const auto Scale{FMath::Min(Canvas->SizeX / (1280.0f * Canvas->GetDPIScale()), Canvas->SizeY / (720.0f * Canvas->GetDPIScale()))};

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{200.0f * Scale};

	auto MaxVerticalLocation{VerticalLocation};
	auto HorizontalLocation{5.0f * Scale};

	if (!DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraCurvesDisplayName()) &&
	    !DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraShapesDisplayName()) &&
	    !DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraTracesDisplayName()))
	{
		VerticalLocation = MaxVerticalLocation;
		return;
	}

	const auto InitialVerticalLocation{VerticalLocation};

	static const auto CameraCurvesHeaderText{FText::AsCultureInvariant(TEXT("Als.CameraCurves (Shift + 6)"))};

	if (DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraCurvesDisplayName()))
	{
		DisplayDebugHeader(Canvas, CameraCurvesHeaderText, FLinearColor::Green, Scale, HorizontalLocation, VerticalLocation);
		DisplayDebugCurves(Canvas, Scale, HorizontalLocation, VerticalLocation);

		MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation + RowOffset);
		VerticalLocation = InitialVerticalLocation;
		HorizontalLocation += ColumnOffset;
	}
	else
	{
		DisplayDebugHeader(Canvas, CameraCurvesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalLocation, VerticalLocation);

		VerticalLocation += RowOffset;
	}

	MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation);

	static const auto ShapesHeaderText{FText::AsCultureInvariant(TEXT("Als.CameraShapes (Shift + 7)"))};

	if (DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraShapesDisplayName()))
	{
		DisplayDebugHeader(Canvas, ShapesHeaderText, FLinearColor::Green, Scale, HorizontalLocation, VerticalLocation);
		DisplayDebugShapes(Canvas, Scale, HorizontalLocation, VerticalLocation);
	}
	else
	{
		DisplayDebugHeader(Canvas, ShapesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalLocation, VerticalLocation);
	}

	VerticalLocation += RowOffset;
	MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation);

	static const auto TracesHeaderText{FText::AsCultureInvariant(TEXT("Als.CameraTraces (Shift + 8)"))};

	if (DebugDisplay.IsDisplayOn(UAlsCameraConstants::CameraTracesDisplayName()))
	{
		DisplayDebugHeader(Canvas, TracesHeaderText, FLinearColor::Green, Scale, HorizontalLocation, VerticalLocation);
		DisplayDebugTraces(Canvas, Scale, HorizontalLocation, VerticalLocation);
	}
	else
	{
		DisplayDebugHeader(Canvas, TracesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalLocation, VerticalLocation);
	}

	VerticalLocation += RowOffset;
	MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation);

	VerticalLocation = MaxVerticalLocation;
}

void UAlsCameraComponent::DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
                                             const float Scale, const float HorizontalLocation, float& VerticalLocation)
{
	FCanvasTextItem Text{
		{HorizontalLocation, VerticalLocation},
		HeaderText,
		GEngine->GetMediumFont(),
		HeaderColor
	};

	Text.Scale = {Scale, Scale};
	Text.EnableShadow(FLinearColor::Black);

	Text.Draw(Canvas->Canvas);

	VerticalLocation += 15.0f * Scale;
}

void UAlsCameraComponent::DisplayDebugCurves(const UCanvas* Canvas, const float Scale,
                                             const float HorizontalLocation, float& VerticalLocation) const
{
	VerticalLocation += 4.0f * Scale;

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
	check(CurveNames.IsEmpty())

	GetAnimInstance()->GetAllCurveNames(CurveNames);

	CurveNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	for (const auto& CurveName : CurveNames)
	{
		const auto CurveValue{GetAnimInstance()->GetCurveValue(CurveName)};

		Text.SetColor(FMath::Lerp(FLinearColor::Gray, FLinearColor::White, UAlsMath::Clamp01(FMath::Abs(CurveValue))));

		Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(CurveName.ToString(), false));
		Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

		Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), CurveValue));
		Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

		VerticalLocation += RowOffset;
	}

	CurveNames.Reset();
}

void UAlsCameraComponent::DisplayDebugShapes(const UCanvas* Canvas, const float Scale,
                                             const float HorizontalLocation, float& VerticalLocation) const
{
	VerticalLocation += 4.0f * Scale;

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
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      PivotTargetLocation.X, PivotTargetLocation.Y, PivotTargetLocation.Z));
	Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

	VerticalLocation += RowOffset;

	static const auto PivotLagLocationText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PivotLagLocation), false))
	};

	Color = {1.0f, 0.5f, 0.0f};
	Text.SetColor(Color);

	Text.Text = PivotLagLocationText;
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      PivotLagLocation.X, PivotLagLocation.Y, PivotLagLocation.Z));
	Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

	VerticalLocation += RowOffset;

	static const auto PivotLocationText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, PivotLocation), false))
	};

	Color = {0.0f, 0.75f, 1.0f};
	Text.SetColor(Color);

	Text.Text = PivotLocationText;
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      PivotLocation.X, PivotLocation.Y, PivotLocation.Z));
	Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

	VerticalLocation += RowOffset;

	static const auto CameraFovText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, CameraFov), false))
	};

	Color = FLinearColor::White;
	Text.SetColor(Color);

	Text.Text = CameraFovText;
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), CameraFov));
	Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

	VerticalLocation += RowOffset;

	static const auto RightShoulderText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, bRightShoulder), true))
	};

	Text.Text = RightShoulderText;
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(UAlsUtility::BoolToString(bRightShoulder), false));
	Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

	VerticalLocation += RowOffset;
}

void UAlsCameraComponent::DisplayDebugTraces(const UCanvas* Canvas, const float Scale,
                                             const float HorizontalLocation, float& VerticalLocation) const
{
	VerticalLocation += 4.0f * Scale;

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
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	VerticalLocation += RowOffset;

	static const auto CameraTraceNoHitText{FText::AsCultureInvariant(TEXT("Camera Trace (No Hit)"))};

	Text.SetColor(FLinearColor::Green);

	Text.Text = CameraTraceNoHitText;
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	VerticalLocation += RowOffset;

	static const auto CameraTraceBlockingHitText{FText::AsCultureInvariant(TEXT("Camera Trace (Blocking Hit)"))};

	Text.SetColor(FLinearColor::Red);

	Text.Text = CameraTraceBlockingHitText;
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	VerticalLocation += RowOffset;
}
