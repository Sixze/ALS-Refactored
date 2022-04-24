#include "AlsCameraAnimationInstance.h"

#include "AlsCameraComponent.h"
#include "AlsCharacter.h"

void UAlsCameraAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AAlsCharacter>(GetOwningActor());
	Camera = Cast<UAlsCameraComponent>(GetSkelMeshComponent());

	if (!GetWorld()->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (Character.IsNull())
		{
			Character = GetMutableDefault<AAlsCharacter>();
		}

		if (Camera.IsNull())
		{
			Camera = GetMutableDefault<UAlsCameraComponent>();
		}
	}
}

void UAlsCameraAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Character.IsNull() || Camera.IsNull())
	{
		return;
	}

	Stance = Character->GetStance();
	Gait = Character->GetGait();
	RotationMode = Character->GetRotationMode();
	ViewMode = Character->GetViewMode();
	LocomotionMode = Character->GetLocomotionMode();
	LocomotionAction = Character->GetLocomotionAction();

	bRightShoulder = Camera->IsRightShoulder();
}
