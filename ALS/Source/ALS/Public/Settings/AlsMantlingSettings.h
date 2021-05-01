#pragma once

#include "AlsMantlingSettings.generated.h"

class UAnimMontage;
class UCurveVector;
class UCurveFloat;

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;

	// Mantling time to interpolation, horizontal and vertical correction amounts curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* InterpolationAndCorrectionAmountsCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartRelativeLocation{-65.0f, 0.0f, -200.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D ReferenceHeight{125.0f, 200.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0))
	FVector2D PlayRate{1.2f, 1.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D StartTime{0.6f, 0.0f};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D LedgeHeight{50.0f, 200.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float ReachDistance{75.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float TargetLocationOffset{15.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	bool bDrawFailedTraces;
};

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* TimelineCurve;

	// If a dynamic object has a speed bigger than this value, then do not start mantling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetPrimitiveSpeedThreshold{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180))
	float MaxReachAngle{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMantlingTraceSettings GroundedTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMantlingTraceSettings InAirTrace{{50.0f, 150.0f}, 70.0f};
};
