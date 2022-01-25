#pragma once

#include "AlsLocomotionAnimationState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float InputYawAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMovingSmooth{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	FVector RelativeAccelerationAmount{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 3))
	float GaitAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GaitWalkingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GaitRunningAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float GaitSprintingAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetForward{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetBackward{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetLeft{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180))
	float RotationYawOffsetRight{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float SprintTime{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float SprintRelativeAccelerationAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float SprintBlockAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowTransitions{false};
};
