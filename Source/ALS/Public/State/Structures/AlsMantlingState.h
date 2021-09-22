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
	UPrimitiveComponent* Primitive{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MantlingHeight{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAlsMantlingType MantlingType{EAlsMantlingType::High};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsMantlingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveVector* InterpolationAndCorrectionCurve{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float WorldStartTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float StartTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float Duration{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* Primitive{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform ActorOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AnimationOffset{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENetworkSmoothingMode PreviousNetworkSmoothingMode{ENetworkSmoothingMode::Disabled};
};
