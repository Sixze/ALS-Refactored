#pragma once

#include "AlsDynamicTransitionSettings.h"
#include "AlsFeetSettings.h"
#include "AlsGeneralAnimationSettings.h"
#include "AlsGroundedSettings.h"
#include "AlsInAirSettings.h"
#include "AlsRotateInPlaceSettings.h"
#include "AlsTurnInPlaceSettings.h"
#include "Engine/DataAsset.h"
#include "AlsAnimationInstanceSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsAnimationInstanceSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsGeneralAnimationSettings General;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsGroundedSettings Grounded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsInAirSettings InAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsDynamicTransitionSettings DynamicTransition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsFeetSettings Feet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsRotateInPlaceSettings RotateInPlace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsGeneralTurnInPlaceSettings TurnInPlace;

public:
	UAlsAnimationInstanceSettings();
};
