#pragma once

#include "AlsDynamicTransitionSettings.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct ALS_API FAlsDynamicTransitionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm"))
	float FootIkDistanceThreshold{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* StandingTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* StandingTransitionRightAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* CrouchingTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* CrouchingTransitionRightAnimation{nullptr};
};
