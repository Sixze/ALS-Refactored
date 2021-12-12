#pragma once

#include "AlsLocomotionState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInput{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float InputYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasSpeed{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float Speed{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PreviousVelocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float VelocityYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoving{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Acceleration{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotationLocked{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float TargetYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float SmoothTargetYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float ViewRelativeTargetYawAngle{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float LastRotationYawSpeedApplyTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastRotationYawSpeed;
};
