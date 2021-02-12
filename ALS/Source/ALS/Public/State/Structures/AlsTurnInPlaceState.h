#pragma once

#include "AlsTurnInPlaceState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsTurnInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActivationDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float PlayRate{1};
};
