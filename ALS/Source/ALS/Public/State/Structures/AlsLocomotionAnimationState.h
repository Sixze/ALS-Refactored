#pragma once

#include "AlsLocomotionAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 3))
	float GaitTypeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float GaitWalkingAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float GaitRunningAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float GaitSprintingAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float InputYawAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float VelocityYawAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTransitionsAllowed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1, ClampMax = 1))
	FVector RelativeAccelerationAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetBackward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float SprintTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1, ClampMax = 1))
	float SprintRelativeAccelerationAmount;
};
