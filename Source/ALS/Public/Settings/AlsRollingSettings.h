#pragma once

#include "Animation/AnimMontage.h"

#include "AlsRollingSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrouchOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateToInputOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float RotationInterpolationSpeed{10.0f};

	// If character landed with a specified speed, then start rolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartRollingOnLand{true};

	// If character landed with a speed greater than specified value, then start rolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, EditCondition = "bStartRollingOnLand"))
	float RollingOnLandSpeedThreshold{600.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInterruptRollingWhenInAir{true};
};
