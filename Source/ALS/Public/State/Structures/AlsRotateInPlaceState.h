#pragma once

#include "AlsRotateInPlaceState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatingLeft{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotatingRight{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FootLockBlockAmount{0.0f};
};
