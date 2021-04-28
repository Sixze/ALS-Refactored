#pragma once

#include "AlsDynamicTransitionSettings.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct ALS_API FAlsDynamicTransitionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float FootIkDistanceThreshold{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* TransitionLeftAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* TransitionRightAnimation;
};
