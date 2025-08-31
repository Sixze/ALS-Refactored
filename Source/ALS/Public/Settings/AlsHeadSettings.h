#pragma once

#include "AlsHeadSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsHeadSettings
{
	GENERATED_BODY()

	// The lower the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float PitchAngleInterpolationHalfLife{0.1f};

	// The lower the value, the faster the interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float YawAngleInterpolationSmoothingTime{0.14f};

	// The lower the value, the faster the interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float SwitchLookSidesYawAngleInterpolationSmoothingTime{0.28f};
};
