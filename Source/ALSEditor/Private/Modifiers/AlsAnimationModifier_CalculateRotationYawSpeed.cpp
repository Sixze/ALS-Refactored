#include "Modifiers/AlsAnimationModifier_CalculateRotationYawSpeed.h"

#include "Animation/AnimSequence.h"
#include "Utility/AlsConstants.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationModifier_CalculateRotationYawSpeed)

void UAlsAnimationModifier_CalculateRotationYawSpeed::OnApply_Implementation(UAnimSequence* Sequence)
{
	Super::OnApply_Implementation(Sequence);

	if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, UAlsConstants::RotationYawSpeedCurveName(), ERawCurveTrackTypes::RCT_Float))
	{
		UAnimationBlueprintLibrary::RemoveCurve(Sequence, UAlsConstants::RotationYawSpeedCurveName());
	}

	UAnimationBlueprintLibrary::AddCurve(Sequence, UAlsConstants::RotationYawSpeedCurveName());

	const auto FrameRate{Sequence->GetSamplingFrameRate().AsDecimal()};

	FTransform CurrentPoseTransform;
	FTransform NextPoseTransform;

	for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
	{
		UAnimationBlueprintLibrary::GetBonePoseForFrame(Sequence, UAlsConstants::RootBoneName(), i, false, CurrentPoseTransform);

		UAnimationBlueprintLibrary::GetBonePoseForFrame(Sequence, UAlsConstants::RootBoneName(), i + (Sequence->RateScale >= 0.0f ? 1 : -1),
		                                                false, NextPoseTransform);

		UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, UAlsConstants::RotationYawSpeedCurveName(), Sequence->GetTimeAtFrame(i),
		                                             UE_REAL_TO_FLOAT(
			                                             NextPoseTransform.Rotator().Yaw - CurrentPoseTransform.Rotator().Yaw) *
		                                             FMath::Abs(Sequence->RateScale) * FrameRate);
	}
}
