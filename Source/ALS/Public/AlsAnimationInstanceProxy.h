#pragma once

#include "Animation/AnimInstanceProxy.h"
#include "AlsAnimationInstanceProxy.generated.h"

class UAlsAnimationInstance;
class UAlsLinkedAnimationInstance;

USTRUCT()
struct ALS_API FAlsAnimationInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	// This allows UAlsAnimationInstance and UAlsLinkedAnimationInstance to access some protected members of FAnimInstanceProxy.

	friend UAlsAnimationInstance;
	friend UAlsLinkedAnimationInstance;

public:
	FAlsAnimationInstanceProxy() = default;

	explicit FAlsAnimationInstanceProxy(UAnimInstance* AnimationInstance);

protected:
	virtual void PostUpdate(UAnimInstance* AnimationInstance) const override;
};
