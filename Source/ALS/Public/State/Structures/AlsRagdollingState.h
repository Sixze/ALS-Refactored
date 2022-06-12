#pragma once

#include "AlsRagdollingState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	int32 SpeedLimitFrameTimeRemaining{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float SpeedLimit{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RootBoneVelocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ForceUnits = "N"))
	float PullForce{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGrounded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFacedUpward{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPendingFinalization{false};
};
