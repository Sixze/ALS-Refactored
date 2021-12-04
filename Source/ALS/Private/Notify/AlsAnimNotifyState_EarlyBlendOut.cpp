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

	// ReSharper disable CppRedundantParentheses
	if ((bCheckInput && Character->GetLocomotionState().bHasInput) ||
	    (bCheckLocomotionMode && Character->GetLocomotionMode() == LocomotionModeEquals) ||
	    (bCheckStance && Character->GetStance() == StanceEquals) ||
	    (bCheckRotationMode && Character->GetRotationMode() == RotationModeEquals))
		// ReSharper restore CppRedundantParentheses
	{
		AnimationInstance->Montage_Stop(BlendOutTime, Montage);
	}
}
