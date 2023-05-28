#include "Notifies/AlsAnimNotifyState_EarlyBlendOut.h"

#include "AlsCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotifyState_EarlyBlendOut)

UAlsAnimNotifyState_EarlyBlendOut::UAlsAnimNotifyState_EarlyBlendOut()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif

	bIsNativeBranchingPoint = true;
}

FString UAlsAnimNotifyState_EarlyBlendOut::GetNotifyName_Implementation() const
{
	return FString{TEXTVIEW("Als Early Blend Out")};
}

void UAlsAnimNotifyState_EarlyBlendOut::NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                   const float DeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(Mesh, Animation, DeltaTime, EventReference);

	const auto* Montage{Cast<UAnimMontage>(Animation)};
	auto* AnimationInstance{IsValid(Montage) ? Mesh->GetAnimInstance() : nullptr};
	const auto* Character{IsValid(AnimationInstance) ? Cast<AAlsCharacter>(Mesh->GetOwner()) : nullptr};

	if (IsValid(Character) &&
	    ((bCheckInput && Character->GetLocomotionState().bHasInput) ||
	     (bCheckLocomotionMode && Character->GetLocomotionMode() == LocomotionModeEquals) ||
	     (bCheckRotationMode && Character->GetRotationMode() == RotationModeEquals) ||
	     (bCheckStance && Character->GetStance() == StanceEquals)))
	{
		AnimationInstance->Montage_Stop(BlendOutDuration, Montage);
	}
}
