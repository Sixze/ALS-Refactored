#include "Modifiers/AlsAnimationModifier_CreateCurves.h"

#include "Animation/AnimSequence.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationModifier_CreateCurves)

void UAlsAnimationModifier_CreateCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);

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
			for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
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
