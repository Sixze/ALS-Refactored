#include "Notify/AlsAnimNotifyState_SetRootMotionScale.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UAlsAnimNotifyState_SetRootMotionScale::UAlsAnimNotifyState_SetRootMotionScale()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_SetRootMotionScale::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Root Motion Scale: {0}"), {TranslationScale});
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation,
                                                         const float TotalDuration)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration);

	auto* Character{Cast<ACharacter>(MeshComponent->GetOwner())};
	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(TranslationScale);
	}
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComponent, Animation);

	auto* Character{Cast<ACharacter>(MeshComponent->GetOwner())};
	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(1.0f);
	}
}
