#include "AlsLinkedAnimationInstance.h"

#include "AlsAnimationInstance.h"
#include "AlsAnimationInstanceProxy.h"
#include "AlsCharacter.h"
#include "Utility/AlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsLinkedAnimationInstance)

UAlsLinkedAnimationInstance::UAlsLinkedAnimationInstance()
{
	RootMotionMode = ERootMotionMode::IgnoreRootMotion;
	bUseMainInstanceMontageEvaluationData = true;
}

void UAlsLinkedAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UAlsAnimationInstance>(GetSkelMeshComponent()->GetAnimInstance());
	Character = Cast<AAlsCharacter>(GetOwningActor());

#if WITH_EDITOR
	if (!GetWorld()->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (!Parent.IsValid())
		{
			Parent = GetMutableDefault<UAlsAnimationInstance>();
		}

		if (!IsValid(Character))
		{
			Character = GetMutableDefault<AAlsCharacter>();
		}
	}
#endif
}

void UAlsLinkedAnimationInstance::NativeBeginPlay()
{
	ALS_ENSURE_MESSAGE(Parent.IsValid(),
	                   TEXT("%s (%s) should only be used as a linked animation instance within the %s animation blueprint!"),
	                   ALS_GET_TYPE_STRING(UAlsLinkedAnimationInstance).GetData(), *GetClass()->GetName(),
	                   ALS_GET_TYPE_STRING(UAlsAnimationInstance).GetData());

	Super::NativeBeginPlay();
}

FAnimInstanceProxy* UAlsLinkedAnimationInstance::CreateAnimInstanceProxy()
{
	return new FAlsAnimationInstanceProxy{this};
}

void UAlsLinkedAnimationInstance::ReinitializeLookTowardsInput()
{
	if (Parent.IsValid())
	{
		Parent->ReinitializeLookTowardsInput();
	}
}

void UAlsLinkedAnimationInstance::RefreshLookTowardsInput()
{
	if (Parent.IsValid())
	{
		Parent->RefreshLookTowardsInput();
	}
}

void UAlsLinkedAnimationInstance::ReinitializeLookTowardsCamera()
{
	if (Parent.IsValid())
	{
		Parent->ReinitializeLookTowardsCamera();
	}
}

void UAlsLinkedAnimationInstance::RefreshLookTowardsCamera()
{
	if (Parent.IsValid())
	{
		Parent->RefreshLookTowardsCamera();
	}
}

void UAlsLinkedAnimationInstance::ResetGroundedEntryMode()
{
	if (Parent.IsValid())
	{
		Parent->ResetGroundedEntryMode();
	}
}

void UAlsLinkedAnimationInstance::SetHipsDirection(const EAlsHipsDirection NewHipsDirection)
{
	if (Parent.IsValid())
	{
		Parent->SetHipsDirection(NewHipsDirection);
	}
}

void UAlsLinkedAnimationInstance::ActivatePivot()
{
	if (Parent.IsValid())
	{
		Parent->ActivatePivot();
	}
}

void UAlsLinkedAnimationInstance::ResetJumped()
{
	if (Parent.IsValid())
	{
		Parent->ResetJumped();
	}
}
