#pragma once

#include "AlsLocomotionAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float InputYawAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMovingSmooth{false};
};
