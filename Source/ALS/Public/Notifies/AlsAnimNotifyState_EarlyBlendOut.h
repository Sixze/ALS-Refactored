#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsAnimNotifyState_EarlyBlendOut.generated.h"

UCLASS(DisplayName = "Als Early Blend Out Animation Notify State")
class ALS_API UAlsAnimNotifyState_EarlyBlendOut : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ForceUnits = "s"))
	float BlendOutDuration{0.25f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bCheckInput{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (InlineEditConditionToggle))
	bool bCheckLocomotionMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (EditCondition = "bCheckLocomotionMode"))
	FGameplayTag LocomotionModeEquals{AlsLocomotionModeTags::InAir};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (InlineEditConditionToggle))
	bool bCheckRotationMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (EditCondition = "bCheckRotationMode"))
	FGameplayTag RotationModeEquals{AlsRotationModeTags::Aiming};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (InlineEditConditionToggle))
	bool bCheckStance{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (EditCondition = "bCheckStance"))
	FGameplayTag StanceEquals{AlsStanceTags::Crouching};

public:
	UAlsAnimNotifyState_EarlyBlendOut();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	                        float DeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
