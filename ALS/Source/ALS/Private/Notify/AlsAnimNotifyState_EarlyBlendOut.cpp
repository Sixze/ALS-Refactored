#include "Notify/AlsAnimNotifyState_EarlyBlendOut.h"

#include "AlsCharacter.h"
#include "Animation/AnimInstance.h"

UAlsAnimNotifyState_EarlyBlendOut::UAlsAnimNotifyState_EarlyBlendOut()
{
	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_EarlyBlendOut::GetNotifyName_Implementation() const
{
	return TEXT("Als Early Blend Out");
}

void UAlsAnimNotifyState_EarlyBlendOut::NotifyTick(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation,
                                                   const float DeltaTime)
{
	Super::NotifyTick(Component, Animation, DeltaTime);

	const auto* Montage{Cast<UAnimMontage>(Animation)};
	if (!IsValid(Montage))
	{
		return;
	}

	auto* AnimInstance{(Component->GetAnimInstance())};
	if (!IsValid(AnimInstance))
	{
		return;
	}

	const auto* Character{Cast<AAlsCharacter>(Component->GetOwner())};
	if (!IsValid(Character))
	{
		return;
	}

	auto bStopMontage{false};

	if (bCheckInput && Character->GetLocomotionState().bHasInput)
	{
		bStopMontage = true;
	}
	else if (bCheckLocomotionMode && Character->GetLocomotionMode() == LocomotionModeEquals)
	{
		bStopMontage = true;
	}
	else if (bCheckStance && Character->GetStance() == StanceEquals)
	{
		bStopMontage = true;
	}
	else if (bCheckRotationMode && Character->GetRotationMode() == RotationModeEquals)
	{
		bStopMontage = true;
	}

	if (bStopMontage)
	{
		AnimInstance->Montage_Stop(BlendOutTime, Montage);
	}
}
