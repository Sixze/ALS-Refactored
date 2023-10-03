#pragma once

#include "AlsRotateInPlaceSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ViewYawAngleThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	FVector2f ReferenceViewYawSpeed{180.0f, 460.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	FVector2f PlayRate{1.15f, 3.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bDisableFootLock{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		Meta = (ClampMin = 0, ClampMax = 180, EditCondition = "!bDisableFootLock", ForceUnits = "deg"))
	float FootLockInhibitionViewYawAngleThreshold{120.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		Meta = (ClampMin = 0, EditCondition = "!bDisableFootLock", ForceUnits = "deg/s"))
	float FootLockInhibitionViewYawSpeedThreshold{620.0f};
};
