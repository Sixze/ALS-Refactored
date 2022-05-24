#pragma once

#include "AlsViewSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnableNetworkSmoothing{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnableListenServerNetworkSmoothing{true};
};
