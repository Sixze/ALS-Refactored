#pragma once

#include "AlsViewSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	bool bEnableNetworkSmoothing{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	bool bEnableListenServerNetworkSmoothing{true};
};
