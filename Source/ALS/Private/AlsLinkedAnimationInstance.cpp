#include "AlsLinkedAnimationInstance.h"

#include "AlsAnimationInstance.h"
#include "Utility/AlsMacro.h"

void UAlsLinkedAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UAlsAnimationInstance>(GetSkelMeshComponent()->GetAnimInstance());
}

void UAlsLinkedAnimationInstance::NativeBeginPlay()
{
	checkf(IsValid(Parent), TEXT("%s (%s) should only be used as a linked animation instance within the %s animation blueprint!"),
	       GET_TYPE_STRING(UAlsLinkedAnimationInstance), *GetClass()->GetName(), GET_TYPE_STRING(UAlsAnimationInstance));

	Super::NativeBeginPlay();
}
