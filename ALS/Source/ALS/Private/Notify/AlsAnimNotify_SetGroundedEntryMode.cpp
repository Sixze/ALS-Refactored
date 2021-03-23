#include "Notify/AlsAnimNotify_SetGroundedEntryMode.h"

#include "AlsAnimationInstance.h"
#include "Utility/AlsEnumerationUtility.h"

UAlsAnimNotify_SetGroundedEntryMode::UAlsAnimNotify_SetGroundedEntryMode()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotify_SetGroundedEntryMode::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Set Grounded Entry Mode: {0}"), {GetEnumValueString(Mode)});
}

void UAlsAnimNotify_SetGroundedEntryMode::Notify(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation)
{
	Super::Notify(Component, Animation);

	auto* AnimInstance{Cast<UAlsAnimationInstance>(Component->GetAnimInstance())};
	if (IsValid(AnimInstance))
	{
		AnimInstance->SetGroundedEntryMode(Mode);
	}
}
