#include "Notify/AlsAnimNotifyState_SetLocomotionAction.h"

#include "AlsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsUtility.h"

UAlsAnimNotifyState_SetLocomotionAction::UAlsAnimNotifyState_SetLocomotionAction()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_SetLocomotionAction::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Locomotion Action: {0}"), {
		                       FName::NameToDisplayString(UAlsUtility::GetSimpleTagName(LocomotionAction).ToString(), false)
	                       });
}

void UAlsAnimNotifyState_SetLocomotionAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                          const float TotalDuration)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration);

	auto* Character{Cast<AAlsCharacter>(Mesh->GetOwner())};
	if (IsValid(Character))
	{
		Character->SetLocomotionAction(LocomotionAction);
	}
}

void UAlsAnimNotifyState_SetLocomotionAction::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(Mesh, Animation);

	auto* Character{Cast<AAlsCharacter>(Mesh->GetOwner())};
	if (IsValid(Character) && Character->GetLocomotionAction() == LocomotionAction)
	{
		Character->SetLocomotionAction(FGameplayTag::EmptyTag);
	}
}
