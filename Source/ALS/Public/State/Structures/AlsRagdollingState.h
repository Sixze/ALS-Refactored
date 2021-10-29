#pragma once

#include "AlsRagdollingState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGrounded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFacedUpward{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RootBoneVelocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PullForce{0.0f};
};
