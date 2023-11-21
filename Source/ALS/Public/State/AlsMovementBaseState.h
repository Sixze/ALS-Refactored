#pragma once

#include "AlsMovementBaseState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsMovementBaseState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UPrimitiveComponent> Primitive{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bBaseChanged : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bHasRelativeLocation : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bHasRelativeRotation : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector Location{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FQuat Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FRotator DeltaRotation{ForceInit};
};
