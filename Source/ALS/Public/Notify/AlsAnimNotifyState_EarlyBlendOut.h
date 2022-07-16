#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsAnimNotifyState_EarlyBlendOut.generated.h"

UCLASS(DisplayName = "Als Early Blend Out Animation Notify State")
class ALS_API UAlsAnimNotifyState_EarlyBlendOut : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, ForceUnits = "s"))
	float BlendOutTime{0.25f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bCheckInput{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, InlineEditConditionToggle))
	bool bCheckLocomotionMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "bCheckLocomotionMode"))
	FGameplayTag LocomotionModeEquals{AlsLocomotionModeTags::InAir};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, InlineEditConditionToggle))
	bool bCheckRotationMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "bCheckRotationMode"))
	FGameplayTag RotationModeEquals{AlsRotationModeTags::Aiming};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, InlineEditConditionToggle))
	bool bCheckStance{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "bCheckStance"))
	FGameplayTag StanceEquals{AlsStanceTags::Crouching};

public:
	UAlsAnimNotifyState_EarlyBlendOut();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	                        float DeltaTime, const FAnimNotifyEventReference& EventReference) override;
};
