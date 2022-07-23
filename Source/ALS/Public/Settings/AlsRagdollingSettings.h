#pragma once

#include "Animation/AnimMontage.h"
#include "AlsRagdollingSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartRagdollingOnLand{true};

	// If a character landed with a speed greater than the specified value, then start ragdolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, EditCondition = "bStartRagdollingOnLand", ForceUnits = "cm/s"))
	float RagdollingOnLandSpeedThreshold{1000.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundTraceObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> GetUpFrontMontage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> GetUpBackMontage{nullptr};
};
