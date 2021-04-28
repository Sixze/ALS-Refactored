#pragma once

#include "AlsAimingCharacterState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsAimingCharacterState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SmoothRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float YawSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float PreviousSmoothYawAngle;
};
