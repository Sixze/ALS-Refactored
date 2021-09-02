#pragma once

#include "AlsRollingSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float ActorRotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrouchOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateToInputOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateToInputDuringRoll{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, EditCondition = "bRotateToInputDuringRoll"))
	float InputInterpolationSpeed{100.0f};

	// If character landed with a specified speed, then start rolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartRollingOnLand{true};

	// If character landed with a speed greater than specified value, then start rolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, EditCondition = "bStartRollingOnLand"))
	float RollingOnLandSpeedThreshold{600.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInterruptRollingWhenInAir{true};
};
