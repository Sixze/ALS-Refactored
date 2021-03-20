#include "Notify/AlsAnimNotifyState_SetRootMotionScale.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

FString UAlsAnimNotifyState_SetRootMotionScale::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Root Motion Scale: {0}"), {TranslationScale});
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyBegin(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation,
                                                         float TotalDuration)
{
	auto* Character{Cast<ACharacter>(Component->GetOwner())};
	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(TranslationScale);
	}
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyEnd(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation)
{
	auto* Character{Cast<ACharacter>(Component->GetOwner())};
	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(1.0f);
	}
}
