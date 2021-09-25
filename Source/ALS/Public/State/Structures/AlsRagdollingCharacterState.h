#pragma once

#include "AlsRagdollingCharacterState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingCharacterState
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
