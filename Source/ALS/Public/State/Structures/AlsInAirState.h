#pragma once

#include "Engine/EngineTypes.h"
#include "AlsInAirState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsInAirState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumped{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float JumpPlayRate{1.0f};

	FTimerHandle JumpResetTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GroundPredictionAmount{1.0f};
};
