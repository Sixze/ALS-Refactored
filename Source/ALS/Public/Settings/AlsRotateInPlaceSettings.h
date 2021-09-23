#pragma once

#include "AlsRotateInPlaceSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180))
	float ViewYawAngleThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D ReferenceViewYawSpeed{180.0f, 460.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D PlayRate{1.15f, 3.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180, EditCondition = "!bDisableFootLock"))
	float FootLockBlockViewYawAngleThreshold{120.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, EditCondition = "!bDisableFootLock"))
	float FootLockBlockViewYawSpeedThreshold{620.0f};
};
