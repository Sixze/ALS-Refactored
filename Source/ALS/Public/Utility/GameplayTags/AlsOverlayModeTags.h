#pragma once

#include "GameplayTagContainer.h"

struct ALS_API FAlsOverlayModeTags : public FGameplayTagNativeAdder
{
private:
	static const FAlsOverlayModeTags Instance;

public:
	FGameplayTag Default;
	FGameplayTag Masculine;
	FGameplayTag Feminine;
	FGameplayTag Injured;
	FGameplayTag HandsTied;
	FGameplayTag M4;
	FGameplayTag PistolOneHanded;
	FGameplayTag PistolTwoHanded;
	FGameplayTag Bow;
	FGameplayTag Torch;
	FGameplayTag Binoculars;
	FGameplayTag Box;
	FGameplayTag Barrel;

public:
	virtual void AddTags() override;

	static const FAlsOverlayModeTags& Get();
};

inline const FAlsOverlayModeTags& FAlsOverlayModeTags::Get()
{
	return Instance;
}
