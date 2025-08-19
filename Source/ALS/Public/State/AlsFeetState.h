#pragma once

#include "AlsFeetState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsFootState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float LockAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f ThighAxis{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector TargetLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat TargetRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector LockLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat LockRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f LockComponentRelativeLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat4f LockComponentRelativeRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f LockMovementBaseRelativeLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat4f LockMovementBaseRelativeRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f FinalLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat4f FinalRotation{ForceInit};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsFeetState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat4f PelvisRotation{ForceInit};

	// Choose whether a foot is planted or about to plant when stopping using the foot planted animation
	// curve. A value less than 0.5 means the foot is planted and a value more than 0.5 means the
	// foot is still in the air. The foot planted curve also determines which foot is planted (or
	// about to plant). Positive values mean the right foot is planted, negative values mean the left.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = -1, ClampMax = 1))
	float FootPlantedAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float FeetCrossingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FAlsFootState Left{
		.ThighAxis = -FVector3f::ZAxisVector
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FAlsFootState Right{
		.ThighAxis = FVector3f::ZAxisVector
	};
};

struct ALS_API FAlsFootUpdateContext
{
	FTransform ComponentTransform;

	FTransform ComponentTransformInverse;

	FAlsFootState* FootState{nullptr};

	float IkAmount{0.0f};

	float LockAmount{0.0f};

	float DeltaTime{0.0f};
};
