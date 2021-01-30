#pragma once

#include "AlsFeetState.generated.h"

USTRUCT(BlueprintType)
struct FAlsFootLockState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseFootLockCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float Amount;
};

USTRUCT(BlueprintType)
struct FAlsFootOffsetState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;
};

USTRUCT(BlueprintType)
struct FAlsFeetState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float IkLeftAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float IkRightAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsFootLockState LockLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsFootLockState LockRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsFootOffsetState OffsetLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsFootOffsetState OffsetRight;

	// Choose whether a foot is planted or about to plant when stopping using the foot planted animation
	// curve. A value less than 0.5 means the foot is planted, and a value more than 0.5 means the
	// foot is still in the air. The foot planted curve also determines which foot is planted (or
	// about to plant). Positive values mean the right foot is planted, negative values mean the left.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1, ClampMax = 1))
	float FootPlanted;

	// Pelvis

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PelvisLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float PelvisOffsetAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 1))
	float IkRootScaleAmount;
};
