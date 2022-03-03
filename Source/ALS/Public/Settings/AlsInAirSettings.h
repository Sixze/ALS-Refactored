#pragma once

#include "Engine/EngineTypes.h"
#include "AlsInAirSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsInAirSettings
{
	GENERATED_BODY()

	// Vertical velocity to lean amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> LeanAmountCurve{nullptr};

	// Ground prediction sweep hit time to ground prediction amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> GroundPredictionAmountCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundPredictionSweepObjectTypes;
};
