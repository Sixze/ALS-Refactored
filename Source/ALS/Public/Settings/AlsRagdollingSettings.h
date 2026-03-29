#pragma once

#include "AlsRagdollingSettings.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bStartRagdollingOnLand : 1 {true};

	/// Сharacter will start ragdolling if they land at a speed that exceeds this value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		Meta = (ClampMin = 0, EditCondition = "bStartRagdollingOnLand", ForceUnits = "cm/s"))
	float RagdollingOnLandSpeedThreshold{1000.0f};

	/// Limits the speed of the ragdoll to the character's last speed for a few frames after activation.
	/// This prevents the ragdoll from reaching an extremely high initial speed when the FPS is
	/// unstable, which occurs when the player jumps and activates the ragdoll simultaneously.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bLimitInitialRagdollSpeed : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> GetUpFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> GetUpBackMontage;
};
