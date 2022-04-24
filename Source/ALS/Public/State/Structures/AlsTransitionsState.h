#pragma once

#include "AlsTransitionsState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowTransitions{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowDynamicTransition{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float DynamicTransitionBlockTimeRemaining{0.0f};
};
