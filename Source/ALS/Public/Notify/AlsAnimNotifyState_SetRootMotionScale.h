#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AlsAnimNotifyState_SetRootMotionScale.generated.h"

UCLASS(DisplayName = "Als Set Root Motion Scale Animation Notify State")
class ALS_API UAlsAnimNotifyState_SetRootMotionScale : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, ClampMin = 0, ForceUnits = "x"))
	float TranslationScale{1.0f};

public:
	UAlsAnimNotifyState_SetRootMotionScale();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	                         float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	                       const FAnimNotifyEventReference& EventReference) override;
};
