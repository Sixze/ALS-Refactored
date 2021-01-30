#pragma once

#include "AlsLocomotionCharacterState.generated.h"

USTRUCT(BlueprintType)
struct FAlsLocomotionCharacterState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetActorRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInputAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float InputAccelerationAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float InputAccelerationYawAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float SmoothMaxAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PreviousVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float VelocityYawAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving;
};
