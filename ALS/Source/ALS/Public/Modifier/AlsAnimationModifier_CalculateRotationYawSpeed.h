#pragma once

#include "AnimationModifier.h"

#include "AlsAnimationModifier_CalculateRotationYawSpeed.generated.h"

// This animation modifier calculates the root rotation speed and is used to create the rotation yaw
// speed curves for the rotation animations. Each curve value represents the rotation speed from
// the current and next frame, so it can be applied to the actors rotation to mimic root motion.
UCLASS(meta = (DisplayName = "Als Calculate Rotation Yaw Speed Animation Modifier"))
class ALS_API UAlsAnimationModifier_CalculateRotationYawSpeed : public UAnimationModifier
{
	GENERATED_BODY()

protected:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
