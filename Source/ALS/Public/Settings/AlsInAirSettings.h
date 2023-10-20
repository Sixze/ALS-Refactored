#pragma once

#include "Engine/EngineTypes.h"
#include "AlsInAirSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsInAirSettings
{
	GENERATED_BODY()

public:
	// Vertical velocity to lean amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> LeanAmountCurve{nullptr};

	// Ground prediction sweep hit time to ground prediction amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> GroundPredictionAmountCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TEnumAsByte<ECollisionChannel> GroundPredictionSweepChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TArray<TEnumAsByte<ECollisionChannel>> GroundPredictionResponseChannels;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FCollisionResponseContainer GroundPredictionSweepResponses{ECR_Ignore};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
