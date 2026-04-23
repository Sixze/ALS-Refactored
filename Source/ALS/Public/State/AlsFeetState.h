#pragma once

#include "AlsFeetState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsFootState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float LockAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f ThighAxisPelvisSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector TargetLocationWorldSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat TargetRotationWorldSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector LockLocationWorldSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat LockRotationWorldSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f LockLocationMovementBaseSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat4f LockRotationMovementBaseSpace{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f LockLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat4f LockRotation{ForceInit};

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

	/// Indicates whether a foot is planted or about to be planted during a stop. A value less than 0.5 means the foot is planted,
	/// while a value greater than 0.5 means the foot is still in the air. It also determines which foot is planted (or about to be
	/// planted). Positive values indicate that the right foot is planted and negative values indicate that the left foot is planted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = -1, ClampMax = 1))
	float FootPlantedAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float FeetCrossingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bValid : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bBecameValid : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FAlsFootState Left{
		.ThighAxisPelvisSpace = -FVector3f::ZAxisVector
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FAlsFootState Right{
		.ThighAxisPelvisSpace = FVector3f::ZAxisVector
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
