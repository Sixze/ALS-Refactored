#pragma once

#include "AlsLeanState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLeanState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float RightAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float ForwardAmount;
};
