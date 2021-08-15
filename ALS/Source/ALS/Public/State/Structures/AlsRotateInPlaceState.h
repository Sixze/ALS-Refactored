#pragma once

#include "AlsRotateInPlaceState.generated.h"

USTRUCT(BlueprintType)
struct FAlsRotateInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatingLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatingRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float InitialPlayRate{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float PlayRate{1};
};
