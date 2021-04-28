#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AlsAnimNotifyState_SetRootMotionScale.generated.h"

enum class EAlsLocomotionAction : uint8;

UCLASS(Meta = (DisplayName = "Als Set Root Motion Scale Animation Notify State"))
class ALS_API UAlsAnimNotifyState_SetRootMotionScale : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, ClampMin = 0))
	float TranslationScale{1.0f};

public:
	UAlsAnimNotifyState_SetRootMotionScale();

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation) override;
};
