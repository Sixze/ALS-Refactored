#pragma once

#include "AlsRollingState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float TargetYawAngle{0.0f};
};
