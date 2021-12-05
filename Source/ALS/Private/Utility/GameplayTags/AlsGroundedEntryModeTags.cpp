#include "Utility/GameplayTags/AlsGroundedEntryModeTags.h"

#include "GameplayTagsManager.h"

const FAlsGroundedEntryModeTags FAlsGroundedEntryModeTags::Instance{};

void FAlsGroundedEntryModeTags::AddTags()
{
	auto& TagsManager{UGameplayTagsManager::Get()};

	Default = TagsManager.AddNativeGameplayTag(TEXT("Als.GroundedEntryMode.Default"));
	FromRoll = TagsManager.AddNativeGameplayTag(TEXT("Als.GroundedEntryMode.FromRoll"));
}
