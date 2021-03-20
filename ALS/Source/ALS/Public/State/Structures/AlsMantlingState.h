#pragma once

#include "Engine/EngineTypes.h"
#include "State/Enumerations/AlsMantlingType.h"

#include "AlsMantlingState.generated.h"

class UAnimMontage;
class UCurveVector;

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* TargetPrimitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector_NetQuantize TargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MantlingHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAlsMantlingType MantlingType;
};

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* InterpolationAndCorrectionCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float PlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* Primitive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform TargetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform ActorOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AnimationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENetworkSmoothingMode PreviousNetworkSmoothingMode;
};
