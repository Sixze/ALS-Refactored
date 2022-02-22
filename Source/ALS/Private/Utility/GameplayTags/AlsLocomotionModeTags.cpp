#include "Utility/GameplayTags/AlsLocomotionModeTags.h"

#include "GameplayTagsManager.h"

const FAlsLocomotionModeTags FAlsLocomotionModeTags::Instance;

void FAlsLocomotionModeTags::AddTags()
{
	auto& TagsManager{UGameplayTagsManager::Get()};

	Grounded = TagsManager.AddNativeGameplayTag(TEXT("Als.LocomotionMode.Grounded"));
	InAir = TagsManager.AddNativeGameplayTag(TEXT("Als.LocomotionMode.InAir"));
}
