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

void UAlsAnimNotifyState_EarlyBlendOut::NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                   const float DeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(Mesh, Animation, DeltaTime, EventReference);

	const auto* Montage{Cast<UAnimMontage>(Animation)};
	auto* AnimationInstance{IsValid(Montage) ? Mesh->GetAnimInstance() : nullptr};
	const auto* Character{IsValid(AnimationInstance) ? Cast<AAlsCharacter>(Mesh->GetOwner()) : nullptr};

	// ReSharper disable CppRedundantParentheses
	if (IsValid(Character) &&
	    ((bCheckInput && Character->GetLocomotionState().bHasInput) ||
	     (bCheckLocomotionMode && Character->GetLocomotionMode() == LocomotionModeEquals) ||
	     (bCheckStance && Character->GetStance() == StanceEquals) ||
	     (bCheckRotationMode && Character->GetRotationMode() == RotationModeEquals)))
	// ReSharper restore CppRedundantParentheses
	{
		AnimationInstance->Montage_Stop(BlendOutTime, Montage);
	}
}
