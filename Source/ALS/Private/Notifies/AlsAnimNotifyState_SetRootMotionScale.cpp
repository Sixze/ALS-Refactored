#include "Notifies/AlsAnimNotifyState_SetRootMotionScale.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Utility/AlsLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotifyState_SetRootMotionScale)

UAlsAnimNotifyState_SetRootMotionScale::UAlsAnimNotifyState_SetRootMotionScale()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif

	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_SetRootMotionScale::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Root Motion Scale: {0}"), {TranslationScale});
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                         const float Duration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, Duration, EventReference);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(TranslationScale);
	}
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                       const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(Mesh, Animation, EventReference);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		if (FMath::IsNearlyEqual(Character->GetAnimRootMotionTranslationScale(), TranslationScale))
		{
			Character->SetAnimRootMotionTranslationScale(1.0f);
		}
		else
		{
			UE_LOG(LogAls, Warning, __FUNCTION__ TEXT(": The current translation scale does not match the translation scale"
				       " from the animation notify! Probably something changed it before the animation notify ended."));
		}
	}
}
