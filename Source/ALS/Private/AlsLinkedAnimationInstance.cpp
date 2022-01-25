#include "AlsLinkedAnimationInstance.h"

#include "AlsAnimationInstance.h"
#include "AlsCharacter.h"
#include "Utility/AlsMacro.h"

UAlsLinkedAnimationInstance::UAlsLinkedAnimationInstance()
{
	RootMotionMode = ERootMotionMode::NoRootMotionExtraction;
}

void UAlsLinkedAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UAlsAnimationInstance>(GetSkelMeshComponent()->GetAnimInstance());
	Character = Cast<AAlsCharacter>(GetOwningActor());
}

void UAlsLinkedAnimationInstance::NativeBeginPlay()
{
	checkf(IsValid(Parent), TEXT("%s (%s) should only be used as a linked animation instance within the %s animation blueprint!"),
	       GET_TYPE_STRING(UAlsLinkedAnimationInstance), *GetClass()->GetName(), GET_TYPE_STRING(UAlsAnimationInstance));

	Super::NativeBeginPlay();
}
