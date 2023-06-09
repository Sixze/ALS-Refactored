#pragma once

#include "Engine/EngineTypes.h"
#include "AlsRagdollingSettings.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bStartRagdollingOnLand{true};

	// If a character landed with a speed greater than the specified value, then start ragdolling.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		Meta = (ClampMin = 0, EditCondition = "bStartRagdollingOnLand", ForceUnits = "cm/s"))
	float RagdollingOnLandSpeedThreshold{1000.0f};

	// If checked, the ragdoll's speed will be limited by the character's last speed for a few frames
	// after activation. This hack is used to prevent the ragdoll from getting a very high initial speed
	// at unstable FPS, which can be reproduced by jumping and activating the ragdoll at the same time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bLimitInitialRagdollSpeed{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundTraceObjectTypes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS")
	FCollisionResponseContainer GroundTraceResponses{ECR_Ignore};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> GetUpFrontMontage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> GetUpBackMontage{nullptr};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
