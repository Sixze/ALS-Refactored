#pragma once

#include "AlsInAirState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsInAirState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ForceUnits = "cm/s"))
	float VerticalVelocity{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumpRequested{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumped{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float JumpPlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GroundPredictionAmount{1.0f};
};
