#include "AlsAnimationModifier_CalculateRotationYawSpeed.h"

#include "Animation/AnimSequence.h"
#include "Utility/AlsConstants.h"

void UAlsAnimationModifier_CalculateRotationYawSpeed::OnApply_Implementation(UAnimSequence* Sequence)
{
	if (UAnimationBlueprintLibrary::DoesCurveExist(Sequence, UAlsConstants::RotationYawSpeedCurve(), ERawCurveTrackTypes::RCT_Float))
	{
		UAnimationBlueprintLibrary::RemoveCurve(Sequence, UAlsConstants::RotationYawSpeedCurve());
	}

	UAnimationBlueprintLibrary::AddCurve(Sequence, UAlsConstants::RotationYawSpeedCurve());

#if ENGINE_MAJOR_VERSION >= 5
	const auto FrameRate{Sequence->GetSamplingFrameRate().AsDecimal()};
#else
	const auto FrameRate{Sequence->GetFrameRate()};
#endif

	FTransform CurrentPoseTransform;
	FTransform NextPoseTransform;

#if ENGINE_MAJOR_VERSION >= 5
	for (auto i{0}; i < Sequence->GetNumberOfSampledKeys(); i++)
#else
	for (auto i{0}; i < Sequence->GetNumberOfFrames(); i++)
#endif
	{
		UAnimationBlueprintLibrary::GetBonePoseForFrame(Sequence, UAlsConstants::RootBone(), i, false, CurrentPoseTransform);

		UAnimationBlueprintLibrary::GetBonePoseForFrame(Sequence, UAlsConstants::RootBone(), i + (Sequence->RateScale >= 0.0f ? 1 : -1),
		                                                false, NextPoseTransform);

		UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, UAlsConstants::RotationYawSpeedCurve(), Sequence->GetTimeAtFrame(i),
		                                             (NextPoseTransform.Rotator().Yaw - CurrentPoseTransform.Rotator().Yaw) *
		                                             FMath::Abs(Sequence->RateScale) * FrameRate);
	}
}
