#include "Notify/AlsAnimNotify_FootstepSound.h"

#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/AlsMath.h"

FString UAlsAnimNotify_FootstepSound::GetNotifyName_Implementation() const
{
	return TEXT("Als Footstep Sound");
}

void UAlsAnimNotify_FootstepSound::Notify(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation)
{
	if (!IsValid(Sound))
	{
		return;
	}

	auto* AnimInstance{(Component->GetAnimInstance())};
	if (!IsValid(AnimInstance))
	{
		return;
	}

	auto ResultVolumeMultiplier{VolumeMultiplier};
	if (!bIgnoreFootstepSoundBlockCurve)
	{
		ResultVolumeMultiplier *= 1.0f - UAlsMath::Clamp01(AnimInstance->GetCurveValue(UAlsConstants::FootstepSoundBlockCurve()));
	}

	auto* Audio{
		UGameplayStatics::SpawnSoundAttached(Sound, Component, AttachPointName, FVector::ZeroVector, FRotator::ZeroRotator,
		                                     EAttachLocation::Type::KeepRelativeOffset, true, ResultVolumeMultiplier, PitchMultiplier)
	};
	if (IsValid(Audio))
	{
		Audio->SetIntParameter(TEXT("FootstepType"), static_cast<int32>(FootstepType));
	}
}
