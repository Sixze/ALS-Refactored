#include "Utility/GameplayTags/AlsOverlayModeTags.h"

#include "GameplayTagsManager.h"

const FAlsOverlayModeTags FAlsOverlayModeTags::Instance{};

void FAlsOverlayModeTags::AddTags()
{
	auto& TagsManager{UGameplayTagsManager::Get()};

	Default = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Default"));
	Masculine = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Masculine"));
	Feminine = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Feminine"));
	Injured = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Injured"));
	HandsTied = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.HandsTied"));
	M4 = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.M4"));
	PistolOneHanded = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.PistolOneHanded"));
	PistolTwoHanded = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.PistolTwoHanded"));
	Bow = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Bow"));
	Torch = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Torch"));
	Binoculars = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Binoculars"));
	Box = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Box"));
	Barrel = TagsManager.AddNativeGameplayTag(TEXT("Als.OverlayMode.Barrel"));
}
