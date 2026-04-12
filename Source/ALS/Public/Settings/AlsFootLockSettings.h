#pragma once

#include "AlsFootLockSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsFootLockSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bAllowFootLock : 1 {true};

	/// Maximum angle by which the vector to the location of the locked foot can differ from the thigh
	/// bone axis. This prevents legs from twisting into a spiral when the character rotates quickly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ThighAngleLimit{90.0f};

	/// Maximum angle by which the rotation of the locked foot can differ from the desired foot rotation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float FootAngleLimit{40.0f};
};
