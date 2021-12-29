#pragma once

#include "AlsInAirRotationMode.h"
#include "AlsMantlingSettings.h"
#include "AlsRagdollingSettings.h"
#include "AlsRollingSettings.h"
#include "Engine/DataAsset.h"

#include "AlsCharacterSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsCharacterSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float MovingSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSprintHasPriorityOverAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRotateToVelocityWhenSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAlsInAirRotationMode InAirRotationMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowAimingWhenInAir{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsGeneralMantlingSettings Mantling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsRagdollingSettings Ragdolling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsRollingSettings Rolling;

public:
	UAlsCharacterSettings();
};
