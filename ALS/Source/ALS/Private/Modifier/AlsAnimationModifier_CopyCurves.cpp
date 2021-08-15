#include "Modifier/AlsAnimationModifier_CopyCurves.h"

#include "Animation/AnimSequence.h"

void UAlsAnimationModifier_CopyCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	if (!IsValid(SourceSequence))
	{
		return;
	}

	TArray<float> CurveTimes;
	TArray<float> CurveValues;

	if (bCopyAllCurves)
	{
		for (const auto& Curve : SourceSequence->RawCurveData.FloatCurves)
		{
			if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, Curve.Name.DisplayName, ERawCurveTrackTypes::RCT_Float))
			{
				UAnimationBlueprintLibrary::RemoveCurve(Sequence, Curve.Name.DisplayName);
			}

			UAnimationBlueprintLibrary::AddCurve(Sequence, Curve.Name.DisplayName);

			UAnimationBlueprintLibrary::GetFloatKeys(SourceSequence, Curve.Name.DisplayName, CurveTimes, CurveValues);
			UAnimationBlueprintLibrary::AddFloatCurveKeys(Sequence, Curve.Name.DisplayName, CurveTimes, CurveValues);
		}
	}
	else
	{
		for (const auto CurveName : CurveNames)
		{
			if (!UAnimationBlueprintLibrary::DoesCurveExist(SourceSequence, CurveName, ERawCurveTrackTypes::RCT_Float))
			{
				continue;
			}

			if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, CurveName, ERawCurveTrackTypes::RCT_Float))
			{
				UAnimationBlueprintLibrary::RemoveCurve(Sequence, CurveName);
			}

			UAnimationBlueprintLibrary::AddCurve(Sequence, CurveName);

			UAnimationBlueprintLibrary::GetFloatKeys(SourceSequence, CurveName, CurveTimes, CurveValues);
			UAnimationBlueprintLibrary::AddFloatCurveKeys(Sequence, CurveName, CurveTimes, CurveValues);
		}
	}
}
