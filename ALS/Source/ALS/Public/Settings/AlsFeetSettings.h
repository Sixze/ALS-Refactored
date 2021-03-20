#pragma once

#include "AlsFeetSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsFeetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float FootHeight{13.5};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IkVerticalCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float IkTraceDistanceUpward{50};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float IkTraceDistanceDownward{45};

	// Forward and backward velocity blend amount to foot ik root bone scale curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* IkRootScaleAmountCurve;
};
