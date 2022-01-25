#pragma once

#include "AlsViewState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float YawSpeed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float PreviousYawAngle{0.0f};
};
