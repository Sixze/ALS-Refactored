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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsGeneralAnimationSettings General;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsGroundedSettings Grounded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsInAirSettings InAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsDynamicTransitionSettings DynamicTransition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsFeetSettings Feet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsRotateInPlaceSettings RotateInPlace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsGeneralTurnInPlaceSettings TurnInPlace;

public:
	UAlsAnimationInstanceSettings();
};
