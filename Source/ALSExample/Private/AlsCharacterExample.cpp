#include "AlsCharacterExample.h"

#include "AlsCameraComponent.h"
#include "TimerManager.h"
#include "Components/InputComponent.h"

AAlsCharacterExample::AAlsCharacterExample()
{
	AlsCamera = CreateDefaultSubobject<UAlsCameraComponent>(TEXT("AlsCamera"));
	AlsCamera->SetupAttachment(GetMesh());
	AlsCamera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});
}

void AAlsCharacterExample::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (!AlsCamera->IsActive())
	{
		Super::CalcCamera(DeltaTime, ViewInfo);
		return;
	}

	AlsCamera->GetViewInfo(ViewInfo);
}

void AAlsCharacterExample::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("LookUpMouse"), this, &ThisClass::InputLookUpMouse);
	PlayerInputComponent->BindAxis(TEXT("LookRightMouse"), this, &ThisClass::InputLookRightMouse);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ThisClass::InputLookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ThisClass::InputLookRight);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::InputMoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::InputMoveRight);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ThisClass::InputSprintPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ThisClass::InputSprintReleased);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_DoubleClick, this, &ThisClass::InputRoll);

	PlayerInputComponent->BindAction(TEXT("Walk"), IE_Pressed, this, &ThisClass::InputWalk);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ThisClass::InputCrouch);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ThisClass::InputJumpPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ThisClass::InputJumpReleased);

	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ThisClass::InputAimPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &ThisClass::InputAimReleased);

	PlayerInputComponent->BindAction(TEXT("Ragdoll"), IE_Pressed, this, &ThisClass::InputRagdollPressed);

	PlayerInputComponent->BindAction(TEXT("RotationMode"), IE_Pressed, this, &ThisClass::InputRotationModePressed);
	PlayerInputComponent->BindAction(TEXT("ViewMode"), IE_Pressed, this, &ThisClass::InputViewModePressed);
	PlayerInputComponent->BindAction(TEXT("SwitchShoulder"), IE_Pressed, this, &ThisClass::InputSwitchShoulderPressed);
}

void AAlsCharacterExample::InputLookUpMouse(const float Value)
{
	AddControllerPitchInput(-Value * LookUpMouseSensitivity);
}

void AAlsCharacterExample::InputLookRightMouse(const float Value)
{
	AddControllerYawInput(Value * LookRightMouseSensitivity);
}

void AAlsCharacterExample::InputLookUp(const float Value)
{
	AddControllerPitchInput(-Value * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAlsCharacterExample::InputLookRight(const float Value)
{
	AddControllerYawInput(Value * LookRightRate * GetWorld()->GetDeltaSeconds());
}

void AAlsCharacterExample::InputMoveForward(const float Value)
{
	AddMovementInput(UAlsMath::AngleToDirection2D(GetViewState().SmoothRotation.Yaw),
	                 UAlsMath::FixGamepadDiagonalValues(Value, GetInputAxisValue(TEXT("MoveRight"))));
}

void AAlsCharacterExample::InputMoveRight(const float Value)
{
	AddMovementInput(UAlsMath::AngleToDirection2D(GetViewState().SmoothRotation.Yaw + 90.0f),
	                 UAlsMath::FixGamepadDiagonalValues(Value, GetInputAxisValue(TEXT("MoveForward"))));
}

void AAlsCharacterExample::InputSprintPressed()
{
	// Start the sprint with a slight delay to give the player enough time to start the roll with a double click instead.

	GetWorldTimerManager().SetTimer(SprintStartTimer, this, &ThisClass::OnSprintStartTimerEnded,
	                                0.1f, false);
}

void AAlsCharacterExample::InputSprintReleased()
{
	if (GetWorldTimerManager().TimerExists(SprintStartTimer))
	{
		GetWorldTimerManager().ClearTimer(SprintStartTimer);
	}
	else
	{
		SetDesiredGait(EAlsGait::Running);
	}
}

void AAlsCharacterExample::OnSprintStartTimerEnded()
{
	SetDesiredGait(EAlsGait::Sprinting);
}

void AAlsCharacterExample::InputRoll()
{
	GetWorldTimerManager().ClearTimer(SprintStartTimer);

	TryStartRolling(1.3f);
}

void AAlsCharacterExample::InputWalk()
{
	if (GetWorldTimerManager().TimerExists(SprintStartTimer))
	{
		return;
	}

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
	SetDesiredAiming(true);
}

void AAlsCharacterExample::InputAimReleased()
{
	SetDesiredAiming(false);
}

void AAlsCharacterExample::InputRagdollPressed()
{
	if (!TryStopRagdolling())
	{
		StartRagdolling();
	}
}

void AAlsCharacterExample::InputRotationModePressed()
{
	SetDesiredRotationMode(GetDesiredRotationMode() != EAlsRotationMode::VelocityDirection
		                       ? EAlsRotationMode::VelocityDirection
		                       : EAlsRotationMode::LookingDirection);
}

void AAlsCharacterExample::InputViewModePressed()
{
	SetViewMode(GetViewMode() == EAlsViewMode::FirstPerson
		            ? EAlsViewMode::ThirdPerson
		            : EAlsViewMode::FirstPerson);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AAlsCharacterExample::InputSwitchShoulderPressed()
{
	AlsCamera->SetRightShoulder(!AlsCamera->IsRightShoulder());
}

void AAlsCharacterExample::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalPosition)
{
	if (AlsCamera->IsActive())
	{
		AlsCamera->DisplayDebug(Canvas, DebugDisplay, VerticalPosition);
	}

	Super::DisplayDebug(Canvas, DebugDisplay, Unused, VerticalPosition);
}
