#include "Modifier/AlsAnimationModifier_CreateLayeringCurves.h"

#include "Animation/AnimSequence.h"

void UAlsAnimationModifier_CreateLayeringCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	for (const auto CurveName : CurveNames)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, CurveName, ERawCurveTrackTypes::RCT_Float))
		{
			UAnimationBlueprintLibrary::RemoveCurve(Sequence, CurveName);
		}

		UAnimationBlueprintLibrary::AddCurve(Sequence, CurveName);

		if (bAddKeyOnEachFrame)
		{
			for (auto i{0}; i < Sequence->GetNumberOfFrames(); i++)
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(i), CurveValue);
			}
		}
		else
		{
			UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(0), CurveValue);
		}
	}
}
