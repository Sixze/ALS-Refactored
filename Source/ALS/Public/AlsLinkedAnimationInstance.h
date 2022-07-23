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
	TObjectPtr<UAlsAnimationInstance> Parent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	TObjectPtr<AAlsCharacter> Character;

public:
	UAlsLinkedAnimationInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

protected:
	// Be very careful when using this function to read your custom variables using the property access system. It is
	// safe to use this function to read variables that change only inside UAlsAnimationInstance::NativeUpdateAnimation()
	// because it is guaranteed that this function will be called before parallel animation evaluation. Reading variables
	// that change in other functions can be dangerous because they can be changed in the game thread at the same
	// time as being read in the worker thread, which can lead to undefined behavior or even a crash. If you're not
	// sure what you're doing, then it's better to access your custom variables through the "Parent" variable.
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Linked Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	UAlsAnimationInstance* GetParentUnsafe() const;
};

inline UAlsAnimationInstance* UAlsLinkedAnimationInstance::GetParentUnsafe() const
{
	return Parent;
}
