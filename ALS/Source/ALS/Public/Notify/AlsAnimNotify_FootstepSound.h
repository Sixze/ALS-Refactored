#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Utility/AlsConstants.h"

#include "AlsAnimNotify_FootstepSound.generated.h"

class USoundBase;

UENUM(BlueprintType)
enum class EAlsFootstepType : uint8
{
	Step,
	WalkRun,
	Jump,
	Land
};

UCLASS(Meta = (DisplayName = "Als Footstep Sound Animation Notify"))
class ALS_API UAlsAnimNotify_FootstepSound : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	float VolumeMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	float PitchMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bIgnoreFootstepSoundBlockCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FName AttachPointName{UAlsConstants::RootBone()};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	EAlsFootstepType FootstepType;

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation) override;
};
