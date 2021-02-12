#pragma once

#include "Components/SkinnedMeshComponent.h"

#include "AlsRagdollingCharacterState.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsRagdollingCharacterState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGrounded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFacedUpward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RootBoneVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PullForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVisibilityBasedAnimTickOption PreviousVisibilityBasedAnimTickOption;
};
