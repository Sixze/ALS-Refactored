#pragma once

#include "Animation/AnimInstanceProxy.h"
#include "AlsAnimationInstanceProxy.generated.h"

class UAlsAnimationInstance;
class UAlsLinkedAnimationInstance;

// This class is only needed to grant UAlsAnimationInstance and UAlsLinkedAnimationInstance
// access to some protected members in FAnimInstanceProxy.
USTRUCT()
struct ALS_API FAlsAnimationInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	friend UAlsAnimationInstance;
	friend UAlsLinkedAnimationInstance;

public:
	FAlsAnimationInstanceProxy() = default;

	explicit FAlsAnimationInstanceProxy(UAnimInstance* AnimationInstance);
};
