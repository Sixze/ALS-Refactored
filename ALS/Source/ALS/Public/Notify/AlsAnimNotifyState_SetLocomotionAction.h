#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AlsAnimNotifyState_SetLocomotionAction.generated.h"

enum class EAlsLocomotionAction : uint8;

UCLASS(Meta = (DisplayName = "Als Set Locomotion Action Animation Notify State"))
class ALS_API UAlsAnimNotifyState_SetLocomotionAction : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	EAlsLocomotionAction Action;

public:
	UAlsAnimNotifyState_SetLocomotionAction();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation) override;
};
