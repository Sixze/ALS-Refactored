#pragma once

#include "AlsAttachment.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsAttachment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Attachment")
	TArray<UStaticMeshComponent*> StaticMeshComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Attachment")
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Attachment")
	TArray<AActor*> Actors;
};
