#include "AlsCharacterExample.h"

#include "Components/InputComponent.h"
#include "Utility/AlsMath.h"

void AAlsCharacterExample::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAlsCharacterExample::InputMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAlsCharacterExample::InputMoveRight);
	PlayerInputComponent->BindAxis("LookUpward", this, &AAlsCharacterExample::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &AAlsCharacterExample::AddControllerYawInput);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AAlsCharacterExample::InputSprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AAlsCharacterExample::InputSprintReleased);
	PlayerInputComponent->BindAction("Sprint", IE_DoubleClick, this, &AAlsCharacterExample::InputRoll);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AAlsCharacterExample::InputWalk);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AAlsCharacterExample::InputCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAlsCharacterExample::InputJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AAlsCharacterExample::InputJumpReleased);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AAlsCharacterExample::InputAimPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AAlsCharacterExample::InputAimReleased);

	PlayerInputComponent->BindAction("RotateToVelocityDirection", IE_Pressed, this, &AAlsCharacterExample::VelocityDirectionPressedAction);
	PlayerInputComponent->BindAction("RotateToLookDirection", IE_Pressed, this, &AAlsCharacterExample::LookingDirectionPressedAction);

	PlayerInputComponent->BindAction("Ragdoll", IE_Pressed, this, &AAlsCharacterExample::RagdollPressedAction);
}

void AAlsCharacterExample::InputMoveForward(const float Value)
{
	AddMovementInput(FVector{UAlsMath::AngleToDirection(GetAimingState().SmoothRotation.Yaw), 0},
	                 UAlsMath::FixGamepadDiagonalValues(Value, GetInputAxisValue("MoveRight")));
}

void AAlsCharacterExample::InputMoveRight(const float Value)
{
	AddMovementInput(FVector{UAlsMath::AngleToDirection(GetAimingState().SmoothRotation.Yaw + 90), 0},
	                 UAlsMath::FixGamepadDiagonalValues(Value, GetInputAxisValue("MoveForward")));
}

void AAlsCharacterExample::InputSprintPressed()
{
	SetDesiredGait(EAlsGait::Sprinting);
}

void AAlsCharacterExample::InputSprintReleased()
{
	SetDesiredGait(EAlsGait::Running);
}

void AAlsCharacterExample::InputRoll()
{
	TryStartRolling(1.3f);
}

void AAlsCharacterExample::InputWalk()
{
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	// ReSharper disable once CppIncompleteSwitchStatement
	switch (GetDesiredGait())
	{
		case EAlsGait::Walking:
			SetDesiredGait(EAlsGait::Running);
			break;

		case EAlsGait::Running:
			SetDesiredGait(EAlsGait::Walking);
			break;
	}
}

void AAlsCharacterExample::InputCrouch()
{
	switch (GetDesiredStance())
	{
		case EAlsStance::Standing:
			SetDesiredStance(EAlsStance::Crouching);
			break;

		case EAlsStance::Crouching:
			SetDesiredStance(EAlsStance::Standing);
			break;
	}
}

void AAlsCharacterExample::InputJumpPressed()
{
	if (TryStopRagdolling())
	{
		return;
	}

	if (TryStartMantlingGrounded())
	{
		return;
	}

	if (GetStance() == EAlsStance::Crouching)
	{
		SetDesiredStance(EAlsStance::Standing);
		return;
	}

	Jump();
}

void AAlsCharacterExample::InputJumpReleased()
{
	StopJumping();
}

void AAlsCharacterExample::InputAimPressed()
{
	SetAiming(true);
}

void AAlsCharacterExample::InputAimReleased()
{
	SetAiming(false);
}

void AAlsCharacterExample::VelocityDirectionPressedAction()
{
	SetDesiredRotationMode(EAlsRotationMode::VelocityDirection);
}

void AAlsCharacterExample::LookingDirectionPressedAction()
{
	SetDesiredRotationMode(EAlsRotationMode::LookingDirection);
}

void AAlsCharacterExample::RagdollPressedAction()
{
	if (!TryStopRagdolling())
	{
		StartRagdolling();
	}
}
