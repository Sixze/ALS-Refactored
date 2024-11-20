#pragma once

#include "Animation/AnimInstanceProxy.h"
#include "AlsAnimationInstanceProxy.generated.h"

USTRUCT()
struct ALS_API FAlsAnimationInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

public:
	FAlsAnimationInstanceProxy() = default;

	explicit FAlsAnimationInstanceProxy(UAnimInstance* AnimationInstance);

protected:
	virtual void PostUpdate(UAnimInstance* AnimationInstance) const override;
};
