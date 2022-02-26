#pragma once

#include "Engine/EngineTypes.h"
#include "AlsTransitionsState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowTransitions{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bAllowDynamicTransition{true};

	FTimerHandle DynamicTransitionAllowanceTimer;
};
