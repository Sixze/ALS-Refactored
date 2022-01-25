#pragma once

#include "Animation/AnimInstance.h"
#include "AlsLinkedAnimationInstance.generated.h"

class UAlsAnimationInstance;
class AAlsCharacter;

UCLASS()
class ALS_API UAlsLinkedAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	UAlsAnimationInstance* Parent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	AAlsCharacter* Character;

public:
	UAlsLinkedAnimationInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;
};
