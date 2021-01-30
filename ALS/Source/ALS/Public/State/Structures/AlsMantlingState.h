#pragma once

#include "AlsMantlingState.generated.h"

class UAnimMontage;
class UCurveVector;

USTRUCT(BlueprintType)
struct FAlsMantlingState
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
};
