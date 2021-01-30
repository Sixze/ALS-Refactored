#pragma once

#include "AlsGeneralSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct FAlsGeneralSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float InputAccelerationYawAmountInterpolationSpeed{8};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AimingSmoothRotationInterpolationSpeed{10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float LeanInterpolationSpeed{4};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetForwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetBackwardCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetLeftCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetRightCurve;
};
