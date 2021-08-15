#pragma once

#include "AlsInAirAnimationState.generated.h"

USTRUCT(BlueprintType)
struct FAlsInAirAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float JumpPlayRate{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VerticalVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float GroundPredictionAmount{1};
};
