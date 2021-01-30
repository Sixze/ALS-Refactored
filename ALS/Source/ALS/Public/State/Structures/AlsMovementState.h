#pragma once

#include "AlsMovementState.generated.h"

UENUM(BlueprintType)
enum class EAlsHipsDirection : uint8
{
	Forward,
	Backward,
	LeftForward,
	LeftBackward,
	RightForward,
	RightBackward,
};

USTRUCT(BlueprintType)
struct FAlsVelocityBlendState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float ForwardAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float BackwardAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float LeftAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float RightAmount;
};

USTRUCT(BlueprintType)
struct FAlsMovementState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAlsHipsDirection HipsDirection = EAlsHipsDirection::Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsVelocityBlendState VelocityBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float StrideBlendAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float WalkRunBlendAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float StandingPlayRate{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float CrouchingPlayRate{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPivotActive;
};
