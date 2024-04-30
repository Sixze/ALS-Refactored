#pragma once

#include "AlsCrouchingState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsCrouchingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float StrideBlendAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 2, ForceUnits = "x"))
	float PlayRate{1.0f};
};
