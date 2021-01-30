#pragma once

#include "AlsAimingCharacterState.generated.h"

USTRUCT(BlueprintType)
struct FAlsAimingCharacterState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SmoothRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float YawSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float PreviousSmoothYawAngle;
};
