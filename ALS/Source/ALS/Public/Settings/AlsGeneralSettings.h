#pragma once

#include "AlsGeneralSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float InputYawAmountInterpolationSpeed{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float ViewSmoothRotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LeanInterpolationSpeed{4.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetForwardCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetBackwardCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetLeftCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RotationYawOffsetRightCurve{nullptr};
};
