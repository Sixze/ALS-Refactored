#pragma once

#include "AnimationModifier.h"
#include "Utility/AlsConstants.h"

#include "AlsAnimationModifier_CreateLayeringCurves.generated.h"

UCLASS(Meta = (DisplayName = "Als Create Layering Curves Animation Modifier"))
class ALSEDITOR_API UAlsAnimationModifier_CreateLayeringCurves : public UAnimationModifier
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bAddKeyOnEachFrame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	float CurveValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	TArray<FName> CurveNames
	{
		UAlsConstants::LayerHeadCurve(),
		UAlsConstants::LayerHeadAdditiveCurve(),
		UAlsConstants::LayerArmLeftCurve(),
		UAlsConstants::LayerArmLeftAdditiveCurve(),
		UAlsConstants::LayerArmLeftLocalSpaceCurve(),
		UAlsConstants::LayerArmRightCurve(),
		UAlsConstants::LayerArmRightAdditiveCurve(),
		UAlsConstants::LayerArmRightLocalSpaceCurve(),
		UAlsConstants::LayerHandLeftCurve(),
		UAlsConstants::LayerHandRightCurve(),
		UAlsConstants::LayerSpineCurve(),
		UAlsConstants::LayerSpineAdditiveCurve(),
		UAlsConstants::LayerPelvisCurve(),
		UAlsConstants::LayerLegsCurve(),

		UAlsConstants::HandLeftIkCurve(),
		UAlsConstants::HandRightIkCurve(),

		UAlsConstants::AimManualCurve(),
		UAlsConstants::AimBlockCurve(),

		UAlsConstants::HipsDirectionLockCurve(),

	};

protected:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
