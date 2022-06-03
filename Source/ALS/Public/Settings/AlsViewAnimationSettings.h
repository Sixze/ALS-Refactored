#pragma once

#include "AlsViewAnimationSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LookTowardsCameraRotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LookTowardsInputYawAngleInterpolationSpeed{10.0f};
};
