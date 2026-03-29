#pragma once

#include "AnimationModifier.h"
#include "AlsAnimationModifier_CalculateRotationYawSpeed.generated.h"

/// This animation modifier calculates the root rotation speed and generates the RotationYawSpeed curve. Each curve value represents
/// the rotation speed between the current and next frames. These values can then be used to rotate the actor and mimic root motion.
UCLASS(DisplayName = "Als Calculate Rotation Yaw Speed Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CalculateRotationYawSpeed : public UAnimationModifier
{
	GENERATED_BODY()

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
