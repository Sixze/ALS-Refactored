#pragma once

#include "AlsGeneralAnimationSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseHandIkBones{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseFootIkBones{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float MovingSmoothSpeedThreshold{150.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LeanInterpolationSpeed{4.0f};
};
