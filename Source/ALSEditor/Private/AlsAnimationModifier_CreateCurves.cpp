#include "AlsAnimationModifier_CreateCurves.h"

#include "Animation/AnimSequence.h"

void UAlsAnimationModifier_CreateCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	for (const auto& Curve : Curves)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, Curve.Name, ERawCurveTrackTypes::RCT_Float))
		{
			if (!bOverrideExistingCurves)
			{
				continue;
			}

			UAnimationBlueprintLibrary::RemoveCurve(Sequence, Curve.Name);
		}

		UAnimationBlueprintLibrary::AddCurve(Sequence, Curve.Name);

		if (Curve.bAddKeyOnEachFrame)
		{
#if ENGINE_MAJOR_VERSION >= 5
			for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
#else
			for (auto i{0}; i < Sequence->GetNumberOfFrames(); i++)
#endif
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, Curve.Name, Sequence->GetTimeAtFrame(i), 0);
			}
		}
		else
		{
			for (const auto& CurveKey : Curve.Keys)
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, Curve.Name, Sequence->GetTimeAtFrame(CurveKey.Frame),
				                                             CurveKey.Value);
			}
		}
	}
}
