#pragma once

#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "State/Enumerations/AlsMantlingType.h"

#include "AlsMantlingSettings.generated.h"

class UAnimMontage;
class UCurveFloat;
class UCurveVector;

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UPrimitiveComponent> TargetPrimitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector_NetQuantize10 TargetRelativeLocation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetRelativeRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MantlingHeight{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAlsMantlingType MantlingType{EAlsMantlingType::High};
};

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsMantlingSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage{nullptr};

	// Mantling time to blend in amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* BlendInCurve{nullptr};

	// Mantling time to interpolation, horizontal and vertical correction amounts curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* InterpolationAndCorrectionAmountsCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartRelativeLocation{-65.0f, 0.0f, -100.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D ReferenceHeight{50.0f, 100.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D StartTime{0.5f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0))
	FVector2D PlayRate{1.0f, 1.0f};

public:
	float CalculateStartTime(float MantlingHeight) const;

	float CalculatePlayRate(float MantlingHeight) const;
};

inline float UAlsMantlingSettings::CalculateStartTime(const float MantlingHeight) const
{
	return FMath::GetMappedRangeValueClamped(ReferenceHeight, StartTime, MantlingHeight);
}

inline float UAlsMantlingSettings::CalculatePlayRate(const float MantlingHeight) const
{
	return FMath::GetMappedRangeValueClamped(ReferenceHeight, PlayRate, MantlingHeight);
}

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D LedgeHeight{50.0f, 225.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float ReachDistance{75.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float TargetLocationOffset{15.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	bool bDrawFailedTraces{false};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowMantling{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180))
	float TraceAngleThreshold{110.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180))
	float MaxReachAngle{50.0f};

	// If a dynamic object has a speed bigger than this value, then do not start mantling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetPrimitiveSpeedThreshold{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMantlingTraceSettings GroundedTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMantlingTraceSettings InAirTrace{{50.0f, 150.0f}, 70.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> MantlingTraceObjectTypes;
};
