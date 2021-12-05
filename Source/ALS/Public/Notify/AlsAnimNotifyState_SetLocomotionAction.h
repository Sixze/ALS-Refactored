#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AlsAnimNotifyState_SetLocomotionAction.generated.h"

UCLASS(DisplayName = "Als Set Locomotion Action Animation Notify State")
class ALS_API UAlsAnimNotifyState_SetLocomotionAction : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FGameplayTag LocomotionAction;

public:
	UAlsAnimNotifyState_SetLocomotionAction();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
