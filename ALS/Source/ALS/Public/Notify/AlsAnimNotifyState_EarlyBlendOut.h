#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "State/Enumerations/AlsLocomotionMode.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"

#include "AlsAnimNotifyState_EarlyBlendOut.generated.h"

class UAnimMontage;

UCLASS(meta = (DisplayName = "Als Early Blend Out Animation Notify State"))
class ALS_API UAlsAnimNotifyState_EarlyBlendOut : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	float BlendOutTime{0.25f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	bool bCheckInputAcceleration{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	bool bCheckLocomotionMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess, EditCondition = "bCheckLocomotionMode"))
	EAlsLocomotionMode LocomotionModeEquals{EAlsLocomotionMode::InAir};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	bool bCheckStance{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess, EditCondition = "bCheckStance"))
	EAlsStance StanceEquals{EAlsStance::Crouching};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	bool bCheckRotationMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess, EditCondition = "bCheckRotationMode"))
	EAlsRotationMode RotationModeEquals{EAlsRotationMode::Aiming};

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyTick(USkeletalMeshComponent* Component, UAnimSequenceBase* Animation, float DeltaTime) override;
};
