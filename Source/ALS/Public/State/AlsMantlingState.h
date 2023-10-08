#pragma once

#include "AlsMantlingState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	int32 RootMotionSourceId = 0;
};
