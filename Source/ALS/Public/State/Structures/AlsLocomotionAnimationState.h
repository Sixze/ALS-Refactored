#pragma once

#include "AlsLocomotionAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInput{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float InputYawAmount{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float Speed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMovingSmooth{false};
};
