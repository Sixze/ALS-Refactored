#pragma once

#include "AlsRollingSettings.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct ALS_API FAlsRollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> Montage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bCrouchOnStart : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bRotateToInputOnStart : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	float RotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bStartRollingOnLand : 1 {true};

	// Rolling will start if the character lands with a speed greater than the specified value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		Meta = (ClampMin = 0, EditCondition = "bStartRollingOnLand", ForceUnits = "cm/s"))
	float RollingOnLandSpeedThreshold{700.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bInterruptRollingWhenInAir : 1 {true};
};
