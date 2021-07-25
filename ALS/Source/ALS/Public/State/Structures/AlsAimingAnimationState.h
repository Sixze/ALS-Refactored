#pragma once

#include "AlsAimingAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsAimingAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float YawAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -90, ClampMax = 90))
	float PitchAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float YawSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SmoothRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float SmoothYawAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -90, ClampMax = 90))
	float SmoothPitchAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float SmoothYawAmount{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 0.5))
	float SmoothYawLeftAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.5, ClampMax = 1))
	float SmoothYawRightAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float PitchAmount{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float LookAmount;
};
