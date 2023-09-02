#include "Modifiers/AlsAnimationModifier_CreateLayeringCurves.h"

#include "Animation/AnimSequence.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationModifier_CreateLayeringCurves)

void UAlsAnimationModifier_CreateLayeringCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);

	CreateCurves(Sequence, CurveNames, CurveValue);

	if (bAddSlotCurves)
	{
		CreateCurves(Sequence, SlotCurveNames, SlotCurveValue);
	}
}

void UAlsAnimationModifier_CreateLayeringCurves::CreateCurves(UAnimSequence* Sequence, const TArray<FName>& Names,
                                                              const float Value) const
{
	for (const auto& CurveName : Names)
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
			for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
			{
				UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(i), Value);
			}
		}
		else
		{
			UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, CurveName, Sequence->GetTimeAtFrame(0), Value);
		}
	}
}
