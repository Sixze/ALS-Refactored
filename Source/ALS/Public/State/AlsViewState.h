#pragma once

#include "AlsViewState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsViewNetworkSmoothingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled{false};

	// Used to track the time of the last server move.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float ServerTime{0.0f};

	// Used to track client time as we try to match the server.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float ClientTime{0.0f};

	// Used for remembering how much time passed between server corrections.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float Duration{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator InitialRotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsViewState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsViewNetworkSmoothingState NetworkSmoothing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float YawSpeed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float PreviousYawAngle{0.0f};
};
