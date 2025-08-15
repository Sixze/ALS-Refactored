#pragma once

#include "AlsViewAnimationSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewAnimationSettings
{
	GENERATED_BODY()

	// The lower the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float LookTowardsCameraRotationInterpolationHalfLife{0.1f};

	// The lower the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float LookTowardsInputYawAngleInterpolationHalfLife{0.1f};
};
