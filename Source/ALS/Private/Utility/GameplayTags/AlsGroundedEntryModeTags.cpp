#include "Utility/GameplayTags/AlsGroundedEntryModeTags.h"

#include "GameplayTagsManager.h"

const FAlsGroundedEntryModeTags FAlsGroundedEntryModeTags::Instance;

void FAlsGroundedEntryModeTags::AddTags()
{
	auto& TagsManager{UGameplayTagsManager::Get()};

	FromRoll = TagsManager.AddNativeGameplayTag(TEXT("Als.GroundedEntryMode.FromRoll"));
}
