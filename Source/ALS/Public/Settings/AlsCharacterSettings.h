#pragma once

#include "AlsInAirRotationMode.h"
#include "AlsMantlingSettings.h"
#include "AlsRagdollingSettings.h"
#include "AlsRollingSettings.h"
#include "AlsViewSettings.h"
#include "Engine/DataAsset.h"
#include "AlsCharacterSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsCharacterSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float MovingSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	bool bSprintHasPriorityOverAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	bool bRotateToVelocityWhenSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	EAlsInAirRotationMode InAirRotationMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	bool bAllowAimingWhenInAir{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	FAlsViewSettings View;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	FAlsGeneralMantlingSettings Mantling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	FAlsRagdollingSettings Ragdolling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS")
	FAlsRollingSettings Rolling;

public:
	UAlsCharacterSettings();
};
