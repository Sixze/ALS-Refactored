#pragma once

#include "AlsViewAnimationSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	float LookTowardsCameraRotationInterpolationSpeed{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	float LookTowardsInputYawAngleInterpolationSpeed{8.0f};
};
