#include "AlsAnimationModifier_CreateLayeringCurves.h"

#include "Animation/AnimSequence.h"

void UAlsAnimationModifier_CreateLayeringCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	for (const auto& CurveName : CurveNames)
	{
		if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, CurveName, ERawCurveTrackTypes::RCT_Float))
		{
			if (!bOverrideExistingCurves)
			{
				continue;
			}

			UAnimationBlueprintLibrary::RemoveCurve(Sequence, CurveName);
		}

		UAnimationBlueprintLibrary::AddCurve(Sequence, CurveName);

		if (bAddKeyOnEachFrame)
		{
#if ENGINE_MAJOR_VERSION >= 5
			for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
#else
			for (auto i{0}; i < Sequence->GetNumberOfFrames(); i++)
#endif
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(i), CurveValue);
			}
		}
		else
		{
			UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(0), CurveValue);
		}
	}

	if (bAddSlotCurves)
	{
		for (const auto& SlotCurveName : SlotCurveNames)
		{
			if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, SlotCurveName, ERawCurveTrackTypes::RCT_Float))
			{
				if (!bOverrideExistingCurves)
				{
					continue;
				}

				UAnimationBlueprintLibrary::RemoveCurve(Sequence, SlotCurveName);
			}

			UAnimationBlueprintLibrary::AddCurve(Sequence, SlotCurveName);

			if (bAddKeyOnEachFrame)
			{
#if ENGINE_MAJOR_VERSION >= 5
				for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
#else
				for (auto i{0}; i < Sequence->GetNumberOfFrames(); i++)
#endif
				{
					UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, SlotCurveName, Sequence->GetTimeAtFrame(i), SlotCurveValue);
				}
			}
			else
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, SlotCurveName, Sequence->GetTimeAtFrame(0), SlotCurveValue);
			}
		}
	}
}
