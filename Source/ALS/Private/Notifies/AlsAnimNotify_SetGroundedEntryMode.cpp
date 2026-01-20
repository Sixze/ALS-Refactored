#include "Notifies/AlsAnimNotify_SetGroundedEntryMode.h"

#include "AlsAnimationInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotify_SetGroundedEntryMode)

UAlsAnimNotify_SetGroundedEntryMode::UAlsAnimNotify_SetGroundedEntryMode()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UAlsAnimNotify_SetGroundedEntryMode::GetNotifyName_Implementation() const
{
	// For some reason editor cuts off some characters at the end of the string, so to avoid this we insert a bunch of spaces.
	// TODO Check the need for this hack in future engine versions.

	TStringBuilder<128> NotifyNameBuilder{
		InPlace, TEXTVIEW("Als Set Grounded Entry Mode: "), GroundedEntryMode.GetTagName(), TEXTVIEW("                ")
	};

	return FString{NotifyNameBuilder};
}

void UAlsAnimNotify_SetGroundedEntryMode::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                                 const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::Notify(Mesh, Sequence, NotifyEventReference);

	auto* AnimationInstance{Cast<UAlsAnimationInstance>(Mesh->GetAnimInstance())};
	if (IsValid(AnimationInstance))
	{
		AnimationInstance->SetGroundedEntryMode(GroundedEntryMode);
	}
}
