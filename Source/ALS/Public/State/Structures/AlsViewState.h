#pragma once

#include "AlsViewState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewState
{
	GENERATED_BODY()

	// Used to track the time of the last server move.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float InterpolationServerTimeStamp{0.0f};

	// Used to track the client time as we try to match the server.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float InterpolationClientTimeStamp{0.0f};

	// Used for remembering how much time has passed between server corrections.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float InterpolationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator InterpolationInitialRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator InterpolationTargetRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float YawSpeed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float PreviousYawAngle{0.0f};
};
