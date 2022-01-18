#pragma once

#include "GameplayTagContainer.h"

struct ALS_API FAlsGroundedEntryModeTags : public FGameplayTagNativeAdder
{
private:
	static const FAlsGroundedEntryModeTags Instance;

public:
	FGameplayTag FromRoll;

public:
	virtual void AddTags() override;

	static const FAlsGroundedEntryModeTags& Get();
};

inline const FAlsGroundedEntryModeTags& FAlsGroundedEntryModeTags::Get()
{
	return Instance;
}
