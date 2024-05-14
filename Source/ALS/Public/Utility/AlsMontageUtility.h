#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsMontageUtility.generated.h"

UCLASS()
class ALS_API UAlsMontageUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Montage Utility", Meta = (ReturnDisplayName = "Transform"))
	static FTransform ExtractRootTransformFromMontage(const UAnimMontage* Montage, float Time);

	UFUNCTION(BlueprintPure, Category = "ALS|Montage Utility", Meta = (ReturnDisplayName = "Transform"))
	static FTransform ExtractLastRootTransformFromMontage(const UAnimMontage* Montage);

	// Stops all active animation montages that share at least one slot with the reference montage.
	UFUNCTION(BlueprintCallable, Category = "ALS|Montage Utility")
	static void StopMontagesWithAnySharedSlots(UAnimInstance* AnimationInstance, const UAnimMontage* ReferenceMontage,
	                                           float BlendOutDuration = -1.0f);
};
