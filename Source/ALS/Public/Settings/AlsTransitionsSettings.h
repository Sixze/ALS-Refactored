#pragma once

#include "AlsTransitionsSettings.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct ALS_API FAlsTransitionsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopBlendInTime{0.1f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopBlendOutTime{0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D QuickStopPlayRate{1.75f, 3.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float QuickStopStartTime{0.3f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> StandingTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> StandingTransitionRightAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> CrouchingTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> CrouchingTransitionRightAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm"))
	float DynamicTransitionFootLockDistanceThreshold{8.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float DynamicTransitionBlendTime{0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float DynamicTransitionPlayRate{1.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> StandingDynamicTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> StandingDynamicTransitionRightAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> CrouchingDynamicTransitionLeftAnimation{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> CrouchingDynamicTransitionRightAnimation{nullptr};
};
