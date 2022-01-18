#pragma once

#include "GameplayTagContainer.h"

struct ALS_API FAlsLocomotionModeTags : public FGameplayTagNativeAdder
{
private:
	static const FAlsLocomotionModeTags Instance;

public:
	FGameplayTag Grounded;
	FGameplayTag InAir;

public:
	virtual void AddTags() override;

	static const FAlsLocomotionModeTags& Get();
};

inline const FAlsLocomotionModeTags& FAlsLocomotionModeTags::Get()
{
	return Instance;
}
