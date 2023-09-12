#include "Modifiers/AlsAnimationModifier_CopyCurves.h"

#include "Animation/AnimSequence.h"
#include "Utility/AlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationModifier_CopyCurves)

void UAlsAnimationModifier_CopyCurves::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);

	auto* SourceSequenceObject{SourceSequence.LoadSynchronous()};
	if (!ALS_ENSURE(IsValid(SourceSequenceObject)))
	{
		return;
	}

	if (bCopyAllCurves)
	{
		for (const auto& Curve : SourceSequenceObject->GetCurveData().FloatCurves)
		{
			CopyCurve(SourceSequenceObject, Sequence, Curve.GetName());
		}
	}
	else
	{
		for (const auto& CurveName : CurveNames)
		{
			if (UAnimationBlueprintLibrary::DoesCurveExist(SourceSequenceObject, CurveName, ERawCurveTrackTypes::RCT_Float))
			{
				CopyCurve(SourceSequenceObject, Sequence, CurveName);
			}
		}
	}
}

void UAlsAnimationModifier_CopyCurves::CopyCurve(UAnimSequence* SourceSequence, UAnimSequence* TargetSequence, const FName& CurveName)
{
	if (UAnimationBlueprintLibrary::DoesCurveExist(TargetSequence, CurveName, ERawCurveTrackTypes::RCT_Float))
	{
		UAnimationBlueprintLibrary::RemoveCurve(TargetSequence, CurveName);
	}

	UAnimationBlueprintLibrary::AddCurve(TargetSequence, CurveName);

	TArray<float> CurveTimes;
	TArray<float> CurveValues;

	UAnimationBlueprintLibrary::GetFloatKeys(SourceSequence, CurveName, CurveTimes, CurveValues);
	UAnimationBlueprintLibrary::AddFloatCurveKeys(TargetSequence, CurveName, CurveTimes, CurveValues);
}
