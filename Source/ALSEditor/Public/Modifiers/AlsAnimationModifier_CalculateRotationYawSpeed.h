#pragma once

#include "AnimationModifier.h"
#include "AlsAnimationModifier_CalculateRotationYawSpeed.generated.h"

// This animation modifier calculates the root rotation speed and is used to create the rotation yaw
// speed curves for the rotation animations. Each curve value represents the rotation speed from
// the current and next frame, so it can be applied to the actors rotation to mimic root motion.
UCLASS(DisplayName = "Als Calculate Rotation Yaw Speed Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CalculateRotationYawSpeed : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
