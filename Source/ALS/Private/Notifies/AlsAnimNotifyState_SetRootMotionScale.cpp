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
}

FString UAlsAnimNotifyState_SetRootMotionScale::GetNotifyName_Implementation() const
{
	TStringBuilder<64> NotifyNameBuilder{InPlace, TEXTVIEW("Als Set Root Motion Scale: ")};

	NotifyNameBuilder.Appendf(TEXT("%.2f"), TranslationScale);

	// For some reason editor cuts off some characters at the end of the string, so to avoid this we insert a bunch of spaces.
	// TODO Check the need for this hack in future engine versions.

	NotifyNameBuilder << TEXTVIEW("        ");

	return FString{NotifyNameBuilder};
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                         const float Duration, const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyBegin(Mesh, Sequence, Duration, NotifyEventReference);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		Character->SetAnimRootMotionTranslationScale(TranslationScale);
	}
}

void UAlsAnimNotifyState_SetRootMotionScale::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                       const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::NotifyEnd(Mesh, Sequence, NotifyEventReference);

	auto* Character{Cast<ACharacter>(Mesh->GetOwner())};

	if (IsValid(Character) && Character->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		if (FMath::IsNearlyEqual(Character->GetAnimRootMotionTranslationScale(), TranslationScale))
		{
			Character->SetAnimRootMotionTranslationScale(1.0f);
		}
		else
		{
			UE_LOG(LogAls, Warning, TEXT("%hs: The current translation scale does not match the translation scale from the")
			       TEXT(" animation notify! Probably something changed it before the animation notify ended."), __FUNCTION__)
		}
	}
}
