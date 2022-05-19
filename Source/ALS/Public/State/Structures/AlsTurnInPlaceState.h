#pragma once

#include "AlsTurnInPlaceState.generated.h"

class UAlsTurnInPlaceSettings;

USTRUCT(BlueprintType)
struct ALS_API FAlsTurnInPlaceState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ActivationDelay{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAlsTurnInPlaceSettings> QueuedSettings{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QueuedSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float QueuedTurnYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFootLockDisabled{false};
};
