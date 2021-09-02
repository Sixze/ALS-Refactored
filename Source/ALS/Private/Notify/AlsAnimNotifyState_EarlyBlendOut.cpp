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

void UAlsAnimNotifyState_EarlyBlendOut::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation,
                                                   const float DeltaTime)
{
	Super::NotifyTick(MeshComponent, Animation, DeltaTime);

	const auto* Montage{Cast<UAnimMontage>(Animation)};
	if (!IsValid(Montage))
	{
		return;
	}

	auto* AnimationInstance{(MeshComponent->GetAnimInstance())};
	if (!IsValid(AnimationInstance))
	{
		return;
	}

	const auto* Character{Cast<AAlsCharacter>(MeshComponent->GetOwner())};
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
		AnimationInstance->Montage_Stop(BlendOutTime, Montage);
	}
}
