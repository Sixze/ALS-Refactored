#pragma once

#include "AlsViewSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	uint8 bEnableNetworkSmoothing : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	uint8 bEnableListenServerNetworkSmoothing : 1 {true};
};
