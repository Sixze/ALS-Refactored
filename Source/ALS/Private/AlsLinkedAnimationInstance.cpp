#include "AlsLinkedAnimationInstance.h"

#include "AlsAnimationInstance.h"
#include "AlsAnimationInstanceProxy.h"
#include "AlsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsLinkedAnimationInstance)

UAlsLinkedAnimationInstance::UAlsLinkedAnimationInstance()
{
	bUseMainInstanceMontageEvaluationData = true;
}

void UAlsLinkedAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UAlsAnimationInstance>(GetSkelMeshComponent()->GetAnimInstance());
	Character = Cast<AAlsCharacter>(GetOwningActor());

#if WITH_EDITOR
	const auto* World{GetWorld()};

	if (IsValid(World) && !World->IsGameWorld())
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
	std::ignore = ALS_ENSURE_MESSAGE(Parent.IsValid(),
	                                 TEXT("%s (%s) should only be used as a linked animation instance within the %s animation blueprint!"),
	                                 ALS_GET_TYPE_STRING(UAlsLinkedAnimationInstance).GetData(), *GetClass()->GetName(),
	                                 ALS_GET_TYPE_STRING(UAlsAnimationInstance).GetData());

	Super::NativeBeginPlay();
}

FAnimInstanceProxy* UAlsLinkedAnimationInstance::CreateAnimInstanceProxy()
{
	return new FAlsAnimationInstanceProxy{this};
}

void UAlsLinkedAnimationInstance::InitializeLook()
{
	if (Parent.IsValid())
	{
		Parent->InitializeLook();
	}
}

void UAlsLinkedAnimationInstance::RefreshLook()
{
	if (Parent.IsValid())
	{
		Parent->RefreshLook();
	}
}

void UAlsLinkedAnimationInstance::InitializeLean()
{
	if (Parent.IsValid())
	{
		Parent->InitializeLean();
	}
}

void UAlsLinkedAnimationInstance::InitializeGrounded()
{
	if (Parent.IsValid())
	{
		Parent->InitializeGrounded();
	}
}

void UAlsLinkedAnimationInstance::RefreshGrounded()
{
	if (Parent.IsValid())
	{
		Parent->RefreshGrounded();
	}
}

void UAlsLinkedAnimationInstance::ResetGroundedEntryMode()
{
	if (Parent.IsValid())
	{
		Parent->ResetGroundedEntryMode();
	}
}

void UAlsLinkedAnimationInstance::RefreshGroundedMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshGroundedMovement();
	}
}

void UAlsLinkedAnimationInstance::SetHipsDirection(const EAlsHipsDirection NewHipsDirection)
{
	if (Parent.IsValid())
	{
		Parent->SetHipsDirection(NewHipsDirection);
	}
}

void UAlsLinkedAnimationInstance::InitializeStandingMovement()
{
	if (Parent.IsValid())
	{
		Parent->InitializeStandingMovement();
	}
}

void UAlsLinkedAnimationInstance::RefreshStandingMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshStandingMovement();
	}
}

void UAlsLinkedAnimationInstance::ResetPivot()
{
	if (Parent.IsValid())
	{
		Parent->ResetPivot();
	}
}

void UAlsLinkedAnimationInstance::RefreshCrouchingMovement()
{
	if (Parent.IsValid())
	{
		Parent->RefreshCrouchingMovement();
	}
}

void UAlsLinkedAnimationInstance::RefreshInAir()
{
	if (Parent.IsValid())
	{
		Parent->RefreshInAir();
	}
}

void UAlsLinkedAnimationInstance::RefreshDynamicTransitions()
{
	if (Parent.IsValid())
	{
		Parent->RefreshDynamicTransitions();
	}
}

void UAlsLinkedAnimationInstance::RefreshRotateInPlace()
{
	if (Parent.IsValid())
	{
		Parent->RefreshRotateInPlace();
	}
}

void UAlsLinkedAnimationInstance::InitializeTurnInPlace()
{
	if (Parent.IsValid())
	{
		Parent->InitializeTurnInPlace();
	}
}

void UAlsLinkedAnimationInstance::RefreshTurnInPlace()
{
	if (Parent.IsValid())
	{
		Parent->RefreshTurnInPlace();
	}
}
