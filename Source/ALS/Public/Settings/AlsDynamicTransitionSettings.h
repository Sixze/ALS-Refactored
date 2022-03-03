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
	TObjectPtr<UAnimSequenceBase> StandingTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> StandingTransitionRightAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> CrouchingTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> CrouchingTransitionRightAnimation{nullptr};
};
