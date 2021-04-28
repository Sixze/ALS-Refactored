#pragma once

#include "AlsRotateInPlaceState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatingLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatingRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float InitialPlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock;
};
