#include "AlsAnimationInstanceProxy.h"

#include "AlsAnimationInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationInstanceProxy)

FAlsAnimationInstanceProxy::FAlsAnimationInstanceProxy(UAnimInstance* AnimationInstance): FAnimInstanceProxy{AnimationInstance} {}

void FAlsAnimationInstanceProxy::PostUpdate(UAnimInstance* AnimationInstance) const
{
	FAnimInstanceProxy::PostUpdate(AnimationInstance);

	// Epic does not allow to override the UAnimInstance::PostUpdateAnimation()
	// function in child classes, so we have to resort to this workaround.

	auto* AlsAnimationInstance{Cast<UAlsAnimationInstance>(AnimationInstance)};
	if (IsValid(AlsAnimationInstance))
	{
		AlsAnimationInstance->NativePostUpdateAnimation();
	}
}
