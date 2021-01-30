#pragma once

#include "AlsMantlingSettings.generated.h"

class UAnimMontage;
class UCurveVector;
class UCurveFloat;

USTRUCT(BlueprintType)
struct FAlsMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;

	// Mantling time to interpolation, horizontal and vertical correction amounts curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* InterpolationAndCorrectionAmountsCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartRelativeLocation{-65, 0, -200};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	FVector2D ReferenceHeight{125, 200};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	FVector2D PlayRate{1.2, 1.2};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	FVector2D StartTime{0.6, 0};
};

USTRUCT(BlueprintType)
struct FAlsMantlingTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	FVector2D LedgeHeight{50, 250};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float ReachDistance{75};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float TraceRadiusForward{30};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float TraceRadiusDownward{30};
};

USTRUCT(BlueprintType)
struct FAlsGeneralMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* TimelineCurve;

	// If a dynamic object has a speed bigger than this value, then do not start mantling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetPrimitiveSpeedThreshold{10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMantlingTraceSettings GroundedTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMantlingTraceSettings InAirTrace{{50, 150}, 70};
};
