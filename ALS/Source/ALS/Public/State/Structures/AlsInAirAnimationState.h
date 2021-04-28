#pragma once

#include "AlsInAirAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsInAirAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float JumpPlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerticalVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GroundPredictionAmount{1.0f};
};
