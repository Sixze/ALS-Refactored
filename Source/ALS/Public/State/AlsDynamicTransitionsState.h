#pragma once

#include "AlsDynamicTransitionsState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsDynamicTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bUpdatedThisFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	int32 FrameDelay{0};
};
