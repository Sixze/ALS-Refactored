#include "Utility/GameplayTags/AlsLocomotionActionTags.h"

#include "GameplayTagsManager.h"

const FAlsLocomotionActionTags FAlsLocomotionActionTags::Instance{};

void FAlsLocomotionActionTags::AddTags()
{
	auto& TagsManager{UGameplayTagsManager::Get()};

	Mantling = TagsManager.AddNativeGameplayTag(TEXT("Als.LocomotionAction.Mantling"));
	Ragdolling = TagsManager.AddNativeGameplayTag(TEXT("Als.LocomotionAction.Ragdolling"));
	GettingUp = TagsManager.AddNativeGameplayTag(TEXT("Als.LocomotionAction.GettingUp"));
	Rolling = TagsManager.AddNativeGameplayTag(TEXT("Als.LocomotionAction.Rolling"));
}
