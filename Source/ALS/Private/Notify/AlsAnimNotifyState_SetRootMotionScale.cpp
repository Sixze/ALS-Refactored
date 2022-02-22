#include "Notify/AlsAnimNotifyState_SetRootMotionScale.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Utility/AlsLog.h"

UAlsAnimNotifyState_SetRootMotionScale::UAlsAnimNotifyState_SetRootMotionScale()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_SetRootMotionScale::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Root Motion Scale: {0}"), {TranslationScale});
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                         const float TotalDuration)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};
	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(TranslationScale);
	}
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(Mesh, Animation);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};
	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		if (FMath::IsNearlyEqual(Character->GetAnimRootMotionTranslationScale(), TranslationScale))
		{
			Character->SetAnimRootMotionTranslationScale(1.0f);
		}
		else
		{
			UE_LOG(LogAls, Warning, TEXT("%s: The current translation scale does not match the translation scale from the animation notify!"
				       " Probably something changed it before the animation notify ended."), ANSI_TO_TCHAR(__FUNCTION__));
		}
	}
}
