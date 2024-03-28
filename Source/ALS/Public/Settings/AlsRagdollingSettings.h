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
	uint8 bStartRagdollingOnLand : 1 {true};

	// Ragdolling will start if the character lands with a speed greater than the specified value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		Meta = (ClampMin = 0, EditCondition = "bStartRagdollingOnLand", ForceUnits = "cm/s"))
	float RagdollingOnLandSpeedThreshold{1000.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TEnumAsByte<ECollisionChannel> GroundTraceChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TArray<TEnumAsByte<ECollisionChannel>> GroundTraceResponseChannels;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FCollisionResponseContainer GroundTraceResponses{ECR_Ignore};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> GetUpFrontMontage{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UAnimMontage> GetUpBackMontage{nullptr};

	// for correction in multiplayer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	float SimulatedProxyMeshInterpolationSpeed{10.0f};

	// If checked, it stops the physical simulation and returns control of the bone to kinematic
	// when the conditions mentioned later are met.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bAllowFreeze : 1 {false};

	// The time until it freezes forcibly after landing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, EditCondition = "bAllowFreeze", ForceUnits = "s"))
	float TimeAfterGroundedForForceFreezing{5.0f};

	// The time until it forcibly freezes after the root bone is considered to have stopped when landing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, EditCondition = "bAllowFreeze", ForceUnits = "s"))
	float TimeAfterGroundedAndStoppedForForceFreezing{1.0f};

	// When the speed is below this value, the root bone is considered to be stopped.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, EditCondition = "bAllowFreeze", ForceUnits = "cm/s"))
	float RootBoneSpeedConsideredAsStopped{5.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, EditCondition = "bAllowFreeze", ForceUnits = "cm/s"))
	float SpeedThresholdToFreeze{5.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, EditCondition = "bAllowFreeze", ForceUnits = "deg"))
	float AngularSpeedThresholdToFreeze{45.0f};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
