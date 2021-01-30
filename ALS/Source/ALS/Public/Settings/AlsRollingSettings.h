#pragma once

#include "AlsRollingSettings.generated.h"

USTRUCT(BlueprintType)
struct FAlsRollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCrouchOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateToInputAccelerationOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSmoothRotateToInputAccelerationDuringRoll{false};

	// If player landed with a specified speed, switch to roll state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartRollingOnLand{true};

	// If player landed with a speed greater than specified value, switch to roll state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, EditCondition = "bStartRollingOnLand"))
	float RollingOnLandSpeedThreshold{600};
};
