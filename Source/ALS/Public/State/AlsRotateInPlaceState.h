#pragma once

#include "AlsRotateInPlaceState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bRotatingLeft{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bRotatingRight{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bFootLockInhibited{false};
};
