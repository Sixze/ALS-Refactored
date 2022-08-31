#pragma once

#include "AlsTransitionsState.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct ALS_API FAlsTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bTransitionsAllowed{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	int32 DynamicTransitionsFrameDelay{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimSequenceBase> QueuedDynamicTransitionAnimation{nullptr};
};
