#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "State/Enumerations/AlsGait.h"
#include "State/Enumerations/AlsLocomotionMode.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"
#include "State/Enumerations/AlsViewMode.h"

#include "AlsCameraAnimationInstance.generated.h"

class AAlsCharacter;
class UAlsCameraComponent;

UCLASS()
class ALSCAMERA_API UAlsCameraAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	AAlsCharacter* AlsCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	UAlsCameraComponent* AlsCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsGait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsViewMode ViewMode{EAlsViewMode::ThirdPerson};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionMode LocomotionMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bRightShoulder{true};

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
