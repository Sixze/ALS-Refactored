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

	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotify_SetGroundedEntryMode::GetNotifyName_Implementation() const
{
	TStringBuilder<256> NotifyNameBuilder;

	NotifyNameBuilder << TEXTVIEW("Als Set Grounded Entry Mode: ")
		<< FName::NameToDisplayString(UAlsUtility::GetSimpleTagName(GroundedEntryMode).ToString(), false);

	return FString{NotifyNameBuilder};
}

void UAlsAnimNotify_SetGroundedEntryMode::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(Mesh, Animation, EventReference);

	auto* AnimationInstance{Cast<UAlsAnimationInstance>(Mesh->GetAnimInstance())};
	if (IsValid(AnimationInstance))
	{
		AnimationInstance->SetGroundedEntryMode(GroundedEntryMode);
	}
}
