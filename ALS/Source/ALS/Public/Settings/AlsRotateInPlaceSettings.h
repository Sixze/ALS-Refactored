#pragma once

#include "AlsRotateInPlaceSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRotateInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180))
	float AimingYawAngleThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D ReferenceAimingYawSpeed{90.0f, 270.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D PlayRate{1.15f, 3.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, EditCondition = "!bDisableFootLock"))
	float MaxFootLockAimingYawSpeed{540.0f};
};
