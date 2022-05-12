#pragma once

#include "AlsViewAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLookTowardsInputState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReinitializationRequired{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float YawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float YawAmount{0.5f};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsLookTowardsCameraState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReinitializationRequired{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float YawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -90, ClampMax = 90, ForceUnits = "deg"))
	float PitchAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float YawForwardAmount{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 0.5))
	float YawLeftAmount{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.5, ClampMax = 1))
	float YawRightAmount{0.5f};
};

USTRUCT(BlueprintType)
struct ALS_API FAlsViewAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float YawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float YawSpeed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -90, ClampMax = 90, ForceUnits = "deg"))
	float PitchAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float PitchAmount{0.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float LookAmount{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float TargetSpineYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float SpineYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsLookTowardsInputState LookTowardsInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsLookTowardsCameraState LookTowardsCamera;
};
