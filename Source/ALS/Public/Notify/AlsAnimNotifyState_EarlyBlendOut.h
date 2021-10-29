#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "State/Enumerations/AlsLocomotionMode.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"

#include "AlsAnimNotifyState_EarlyBlendOut.generated.h"

class UAnimMontage;

UCLASS(DisplayName = "Als Early Blend Out Animation Notify State")
class ALS_API UAlsAnimNotifyState_EarlyBlendOut : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	float BlendOutTime{0.25f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bCheckInput{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bCheckLocomotionMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "bCheckLocomotionMode"))
	EAlsLocomotionMode LocomotionModeEquals{EAlsLocomotionMode::InAir};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bCheckStance{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "bCheckStance"))
	EAlsStance StanceEquals{EAlsStance::Crouching};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bCheckRotationMode{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "bCheckRotationMode"))
	EAlsRotationMode RotationModeEquals{EAlsRotationMode::Aiming};

public:
	UAlsAnimNotifyState_EarlyBlendOut();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float DeltaTime) override;
};
