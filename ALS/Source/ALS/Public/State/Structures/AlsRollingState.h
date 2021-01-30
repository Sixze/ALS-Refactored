#pragma once

#include "AlsRollingState.generated.h"

USTRUCT(BlueprintType)
struct FAlsRollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float TargetYawAngle;
};
