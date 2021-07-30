#pragma once

#include "AlsInAirSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsInAirSettings
{
	GENERATED_BODY()

	// Ground prediction sweep hit time to ground prediction amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* GroundPredictionAmountCurve{nullptr};

	// Vertical velocity to lean amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* LeanAmountCurve{nullptr};
};
