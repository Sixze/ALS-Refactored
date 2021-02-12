#pragma once

#include "AlsRagdollingAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float FlailPlayRate;
};
