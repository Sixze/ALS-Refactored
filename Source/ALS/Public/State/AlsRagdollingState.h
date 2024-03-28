#pragma once

#include "AlsRagdollingState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ForceUnits = "deg"))
	float LyingDownYawAngleDelta{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bGrounded : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bFacingUpward : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ForceUnits = "s"))
	float ElapsedTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ForceUnits = "s"))
	float TimeAfterGrounded{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ForceUnits = "s"))
	float TimeAfterGroundedAndStopped{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector PrevActorLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bFreezing : 1 {false};

	// Speed of root bone
	// Exists solely for display in the editor. Useful when determining setting value.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS", Meta = (ForceUnits = "cm/s"))
	float RootBoneSpeed{0.0f};

	// Highest speed among all bodies
	// Exists solely for display in the editor. Useful when determining setting value.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS", Meta = (ForceUnits = "cm/s"))
	float MaxBoneSpeed{0.0f};

	// Highest angular speed among all bodies
	// Exists solely for display in the editor. Useful when determining setting value.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ALS", Meta = (ForceUnits = "deg"))
	float MaxBoneAngularSpeed{0.0f};
};
