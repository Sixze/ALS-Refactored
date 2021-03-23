#include "Notify/AlsAnimNotifyState_SetLocomotionAction.h"

#include "AlsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "State/Enumerations/AlsLocomotionAction.h"
#include "Utility/AlsEnumerationUtility.h"

UAlsAnimNotifyState_SetLocomotionAction::UAlsAnimNotifyState_SetLocomotionAction()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_SetLocomotionAction::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Locomotion Action: {0}"), {GetEnumValueString(Action)});
}

void UAlsAnimNotifyState_SetLocomotionAction::NotifyBegin(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation,
                                                          const float TotalDuration)
{
	Super::NotifyBegin(Component, Animation, TotalDuration);

	auto* Character{Cast<AAlsCharacter>(Component->GetOwner())};
	if (IsValid(Character))
	{
		Character->SetLocomotionAction(Action);
	}
}

void UAlsAnimNotifyState_SetLocomotionAction::NotifyEnd(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(Component, Animation);

	auto* Character{Cast<AAlsCharacter>(Component->GetOwner())};
	if (IsValid(Character) && Character->GetLocomotionAction() == Action)
	{
		Character->SetLocomotionAction(EAlsLocomotionAction::None);
	}
}
