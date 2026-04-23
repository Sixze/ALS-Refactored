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

	const auto* DataModel{Sequence->GetDataModel()};
	const auto FrameRate{Sequence->GetSamplingFrameRate().AsDecimal()};

	UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, UAlsConstants::RotationYawSpeedCurveName(), 0.0f, 0.0f);

	for (auto i{1}; i < Sequence->GetNumberOfSampledKeys(); i++)
	{
		const auto RootBoneRotation{
			DataModel->GetBoneTrackTransform(UAlsConstants::RootBoneName(), i + (Sequence->RateScale >= 0.0f ? -1 : 0)).GetRotation()
		};

		const auto NextRootBoneRotation{
			DataModel->GetBoneTrackTransform(UAlsConstants::RootBoneName(), i + (Sequence->RateScale >= 0.0f ? 0 : -1)).GetRotation()
		};

		const auto DeltaYawAngle{
			FMath::RadiansToDegrees((NextRootBoneRotation * RootBoneRotation.Inverse()).GetTwistAngle(FVector::UpVector))
		};

		UAnimationBlueprintLibrary::AddFloatCurveKey(Sequence, UAlsConstants::RotationYawSpeedCurveName(), Sequence->GetTimeAtFrame(i),
		                                             UE_REAL_TO_FLOAT(DeltaYawAngle * FMath::Abs(Sequence->RateScale) * FrameRate));
	}
}
