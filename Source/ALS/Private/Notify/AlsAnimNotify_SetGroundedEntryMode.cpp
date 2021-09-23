#include "Notify/AlsAnimNotify_SetGroundedEntryMode.h"

#include "AlsAnimationInstance.h"
#include "Utility/AlsUtility.h"

UAlsAnimNotify_SetGroundedEntryMode::UAlsAnimNotify_SetGroundedEntryMode()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotify_SetGroundedEntryMode::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Grounded Entry Mode: {0}"), {
		                       FName::NameToDisplayString(UAlsUtility::GetSimpleTagName(GroundedEntryMode).ToString(), false)
	                       });
}

void UAlsAnimNotify_SetGroundedEntryMode::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComponent, Animation);

	auto* AnimationInstance{Cast<UAlsAnimationInstance>(MeshComponent->GetAnimInstance())};
	if (IsValid(AnimationInstance))
	{
		AnimationInstance->SetGroundedEntryMode(GroundedEntryMode);
	}
}
