#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AlsAnimNotify_SetGroundedEntryMode.generated.h"

UCLASS(DisplayName = "Als Set Grounded Entry Mode Animation Notify", Meta = (ShowWorldContextPin))
class ALS_API UAlsAnimNotify_SetGroundedEntryMode : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FGameplayTag GroundedEntryMode;

public:
	UAlsAnimNotify_SetGroundedEntryMode();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation) override;
};
