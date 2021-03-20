#pragma once

#include "AlsRotateInPlaceSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 180))
	float AimingYawAngleThreshold{50};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	FVector2D ReferenceAimingYawSpeed{90, 270};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	FVector2D PlayRate{1.15, 3};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, EditCondition = "!bDisableFootLock"))
	float MaxFootLockAimingYawSpeed{540};
};
