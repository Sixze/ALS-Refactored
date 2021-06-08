#pragma once

#include "Engine/EngineTypes.h"

#include "AlsFeetSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsFeetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float FootHeight{13.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IkVerticalCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> IkTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float IkTraceDistanceUpward{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float IkTraceDistanceDownward{45.0f};

	// Forward and backward velocity blend amount to foot ik root bone scale curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* IkRootScaleAmountCurve;
};
