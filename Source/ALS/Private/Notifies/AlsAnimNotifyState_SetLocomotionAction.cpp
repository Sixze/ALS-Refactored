#include "Notifies/AlsAnimNotifyState_SetLocomotionAction.h"

#include "AlsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotifyState_SetLocomotionAction)

UAlsAnimNotifyState_SetLocomotionAction::UAlsAnimNotifyState_SetLocomotionAction()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_SetLocomotionAction::GetNotifyName_Implementation() const
{
	TStringBuilder<256> NotifyNameBuilder{
		InPlace, TEXTVIEW("Als Set Locomotion Action: "),
		FName::NameToDisplayString(UAlsUtility::GetSimpleTagName(LocomotionAction).ToString(), false)
	};

	return FString{NotifyNameBuilder};
}

void UAlsAnimNotifyState_SetLocomotionAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                          const float Duration, const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyBegin(Mesh, Sequence, Duration, NotifyEventReference);

	auto* Character{Cast<AAlsCharacter>(Mesh->GetOwner())};
	if (IsValid(Character))
	{
		Character->SetLocomotionAction(LocomotionAction);
	}
}

void UAlsAnimNotifyState_SetLocomotionAction::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                        const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyEnd(Mesh, Sequence, NotifyEventReference);

	auto* Character{Cast<AAlsCharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocomotionAction() == LocomotionAction)
	{
		Character->SetLocomotionAction(FGameplayTag::EmptyTag);
	}
}
