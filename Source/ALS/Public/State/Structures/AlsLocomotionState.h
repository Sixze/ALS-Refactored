#pragma once

#include "AlsLocomotionState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInput{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float InputYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasSpeed{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float Speed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PreviousVelocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float VelocityYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Acceleration{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotationLocked{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float TargetYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float ViewRelativeTargetYawAngle{0.0f};

	// Used for extra smooth actor rotation, in other cases equal to the regular target yaw angle.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float SmoothTargetYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat RotationQuaternion{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float PreviousYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ForceUnits = "deg/s"))
	float YawSpeed{0.0f};
};
