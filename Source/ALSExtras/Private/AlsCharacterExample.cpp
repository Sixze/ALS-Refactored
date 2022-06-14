#include "AlsCharacterExample.h"

#include "AlsCameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

AAlsCharacterExample::AAlsCharacterExample()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});
}

void AAlsCharacterExample::NotifyControllerChanged()
{
	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* EnhancedInputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())};
		if (IsValid(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* EnhancedInputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())};
		if (IsValid(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	Super::NotifyControllerChanged();
}

void AAlsCharacterExample::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}

void AAlsCharacterExample::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::InputLookMouse);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::InputSprint);
		EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::InputWalk);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::InputCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::InputJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::InputAim);
		EnhancedInput->BindAction(RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::InputRagdoll);
		EnhancedInput->BindAction(RollAction, ETriggerEvent::Triggered, this, &ThisClass::InputRoll);
		EnhancedInput->BindAction(RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::InputRotationMode);
		EnhancedInput->BindAction(ViewModeAction, ETriggerEvent::Triggered, this, &ThisClass::InputViewMode);
		EnhancedInput->BindAction(SwitchShoulderAction, ETriggerEvent::Triggered, this, &ThisClass::InputSwitchShoulder);
	}
}

void AAlsCharacterExample::InputLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

void AAlsCharacterExample::InputLook(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpRate * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(Value.X * LookRightRate * GetWorld()->GetDeltaSeconds());
}

void AAlsCharacterExample::InputMove(const FInputActionValue& ActionValue)
{
	const auto Value{UAlsMath::ClampMagnitude012D(ActionValue.Get<FVector2D>())};

	const auto ForwardDirection{UAlsMath::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	const auto RightDirection{UAlsMath::PerpendicularCounterClockwiseXY(ForwardDirection)};

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void AAlsCharacterExample::InputSprint(const FInputActionValue& ActionValue)
{
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void AAlsCharacterExample::InputWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void AAlsCharacterExample::InputCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

void AAlsCharacterExample::InputJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (TryStopRagdolling())
		{
			return;
		}

		if (TryStartMantlingGrounded())
		{
			return;
		}

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Jump();
	}
	else
	{
		StopJumping();
	}
}

void AAlsCharacterExample::InputAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

void AAlsCharacterExample::InputRagdoll()
{
	if (!TryStopRagdolling())
	{
		StartRagdolling();
	}
}

void AAlsCharacterExample::InputRoll()
{
	static constexpr auto PlayRate{1.3f};

	TryStartRolling(PlayRate);
}

void AAlsCharacterExample::InputRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
		                       ? AlsRotationModeTags::LookingDirection
		                       : AlsRotationModeTags::VelocityDirection);
}

void AAlsCharacterExample::InputViewMode()
{
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AAlsCharacterExample::InputSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void AAlsCharacterExample::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DebugDisplay, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DebugDisplay, Unused, VerticalLocation);
}
