#pragma once

#include "AlsGeneralSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float InputYawAmountInterpolationSpeed{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AimingSmoothRotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float LeanInterpolationSpeed{4.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetForwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetBackwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetLeftCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetRightCurve;
};
