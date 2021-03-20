#pragma once

#include "AlsRagdollingSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartRagdollingOnLand{true};

	// If player landed with a speed greater than specified value, then start ragdolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bStartRagdollingOnLand"))
	float RagdollingOnLandSpeedThreshold{1000.0f};
};
