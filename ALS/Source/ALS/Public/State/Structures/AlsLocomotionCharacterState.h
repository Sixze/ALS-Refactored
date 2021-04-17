#pragma once

#include "AlsLocomotionCharacterState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionCharacterState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float InputYawAngle;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetActorRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SmoothLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SmoothRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator PreviousSmoothRotation;
};
