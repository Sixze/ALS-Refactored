#pragma once

#include "AlsViewSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsViewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LookTowardsCameraRotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LookTowardsInputYawAngleInterpolationSpeed{8.0f};
};
