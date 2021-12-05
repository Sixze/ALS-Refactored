#pragma once

#include "GameplayTagContainer.h"

struct ALS_API FAlsLocomotionActionTags : public FGameplayTagNativeAdder
{
private:
	static const FAlsLocomotionActionTags Instance;

public:
	FGameplayTag Mantling;
	FGameplayTag Ragdolling;
	FGameplayTag GettingUp;
	FGameplayTag Rolling;

public:
	virtual void AddTags() override;

	static const FAlsLocomotionActionTags& Get();
};

inline const FAlsLocomotionActionTags& FAlsLocomotionActionTags::Get()
{
	return Instance;
}
