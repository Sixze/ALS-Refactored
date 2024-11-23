#pragma once

#include "AlsGroundedSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsGroundedSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetForwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetBackwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetLeftCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> RotationYawOffsetRightCurve;

	// The higher the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	float VelocityBlendInterpolationSpeed{12.0f};
};
