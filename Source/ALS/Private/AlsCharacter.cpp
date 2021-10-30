#include "AlsCharacter.h"

#include "AlsAnimationInstance.h"
#include "AlsCharacterMovementComponent.h"
#include "TimerManager.h"
#include "Curves/CurveFloat.h"
#include "Engine/CollisionProfile.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsLog.h"
#include "Utility/AlsMath.h"

AAlsCharacter::AAlsCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAlsCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	GetMesh()->SetRelativeLocation_Direct({0.0f, 0.0f, -90.0f});
	GetMesh()->SetRelativeRotation_Direct({0.0f, -90.0f, 0.0f});
	GetMesh()->SetAnimInstanceClass(UAlsAnimationInstance::StaticClass());

	// Required for the flail animation to work correctly when ragdolling.

	GetMesh()->bUpdateJointsFromAnimation = true;

	// If this option is enabled, then it can cause problems with animation curves (for example, aiming will not work correctly).
	// https://answers.unrealengine.com/questions/1001006/view.html

	GetMesh()->bEnableUpdateRateOptimizations = false;

	AlsCharacterMovement = Cast<UAlsCharacterMovementComponent>(GetCharacterMovement());

	RagdollingSettings.GroundTraceObjectTypes =
	{
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};

	GeneralMantlingSettings.MantlingTraceObjectTypes =
	{
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};
}

void AAlsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;

	Parameters.Condition = COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, DesiredStance, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, DesiredGait, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bDesiredAiming, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, DesiredRotationMode, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ViewMode, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, OverlayMode, Parameters)

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InputDirection, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ViewRotation, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RagdollTargetLocation, Parameters)
}

void AAlsCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Make sure the mesh and animation blueprint update after the character to ensure it gets the most recent values.

	GetMesh()->AddTickPrerequisiteActor(this);

	// Pass current movement settings to the movement component.

	AlsCharacterMovement->SetMovementSettings(MovementSettings);

	// Set default rotation values.

	RefreshLocomotionLocationAndRotation();
	RefreshTargetYawAngle(LocomotionState.Rotation.Yaw);

	LocomotionState.InputYawAngle = LocomotionState.Rotation.Yaw;
	LocomotionState.VelocityYawAngle = LocomotionState.Rotation.Yaw;

	ViewState.SmoothRotation = ViewRotation;
	ViewState.PreviousSmoothYawAngle = ViewRotation.Yaw;
}

void AAlsCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Update states to use the initial desired values.

	RefreshRotationMode();

	AlsCharacterMovement->SetRotationMode(RotationMode);

	ApplyDesiredStance();

	AlsCharacterMovement->SetStance(Stance);

	RefreshGait();

	OnOverlayModeChanged(OverlayMode);
}

void AAlsCharacter::Restart()
{
	Super::Restart();

	ApplyDesiredStance();
}

void AAlsCharacter::Tick(const float DeltaTime)
{
	// Restore initial visibility based animation tick option.

	GetMesh()->VisibilityBasedAnimTickOption = GetClass()->GetDefaultObject<ACharacter>()->GetMesh()->VisibilityBasedAnimTickOption;

	Super::Tick(DeltaTime);

	RefreshRotationMode();

	RefreshLocomotion(DeltaTime);
	RefreshView(DeltaTime);

	// ReSharper disable once CppIncompleteSwitchStatement
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch (LocomotionMode)
	{
		case EAlsLocomotionMode::Grounded:
			RefreshGait();
			RefreshGroundedActorRotation(DeltaTime);
			break;

		case EAlsLocomotionMode::InAir:
			RefreshInAirActorRotation(DeltaTime);
			TryStartMantlingInAir();
			break;

		case EAlsLocomotionMode::Mantling:
			RefreshMantling();
			break;

		case EAlsLocomotionMode::Ragdolling:
			RefreshRagdolling(DeltaTime);
			break;
	}

	LocomotionState.PreviousVelocity = LocomotionState.Velocity;
	ViewState.PreviousSmoothYawAngle = ViewState.SmoothRotation.Yaw;
}

void AAlsCharacter::AddMovementInput(const FVector Direction, const float Scale, const bool bForce)
{
	if (LocomotionMode == EAlsLocomotionMode::Grounded ||
	    LocomotionMode == EAlsLocomotionMode::InAir)
	{
		Super::AddMovementInput(Direction, Scale, bForce);
	}
}

void AAlsCharacter::Jump()
{
	if (LocomotionMode == EAlsLocomotionMode::Grounded &&
	    LocomotionAction == EAlsLocomotionAction::None &&
	    Stance == EAlsStance::Standing)
	{
		Super::Jump();
	}
}

void AAlsCharacter::OnMovementModeChanged(const EMovementMode PreviousMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMode, PreviousCustomMode);

	// Use the character movement mode to set the locomotion mode to the right value. This allows you to have a
	// custom set of movement modes but still use the functionality of the default character movement component.

	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch (AlsCharacterMovement->MovementMode)
	{
		case MOVE_Walking:
		case MOVE_NavWalking:
			SetLocomotionMode(EAlsLocomotionMode::Grounded);
			break;

		case MOVE_Falling:
			SetLocomotionMode(EAlsLocomotionMode::InAir);
			break;
	}
}

void AAlsCharacter::OnStartCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (LocomotionAction == EAlsLocomotionAction::Rolling)
	{
		SetStance(DesiredStance); // Keep desired stance when rolling.
		return;
	}

	SetStance(EAlsStance::Crouching);
}

void AAlsCharacter::OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(EAlsStance::Standing);
}

void AAlsCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	if (IsLocallyControlled())
	{
		OnJumpedNetworked();
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastOnJumpedNetworked();
	}
}

void AAlsCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (IsLocallyControlled())
	{
		OnLandedNetworked();
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastOnLandedNetworked();
	}
}

void AAlsCharacter::SetDesiredStance(const EAlsStance NewStance)
{
	if (DesiredStance != NewStance)
	{
		DesiredStance = NewStance;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, DesiredStance, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetDesiredStance(NewStance);
		}

		ApplyDesiredStance();
	}
}

void AAlsCharacter::ServerSetDesiredStance_Implementation(const EAlsStance NewStance)
{
	SetDesiredStance(NewStance);
}

void AAlsCharacter::ApplyDesiredStance()
{
	if (LocomotionAction == EAlsLocomotionAction::None)
	{
		if (LocomotionMode == EAlsLocomotionMode::Grounded)
		{
			switch (DesiredStance)
			{
				case EAlsStance::Standing:
					UnCrouch();
					break;

				case EAlsStance::Crouching:
					Crouch();
					break;
			}
		}
		else if (LocomotionMode == EAlsLocomotionMode::InAir)
		{
			UnCrouch();
		}
	}
	else if (LocomotionAction == EAlsLocomotionAction::Rolling && RollingSettings.bCrouchOnStart)
	{
		Crouch();
	}
}

void AAlsCharacter::SetStance(const EAlsStance NewStance)
{
	AlsCharacterMovement->SetStance(NewStance);

	if (Stance != NewStance)
	{
		const auto PreviousStance{Stance};

		Stance = NewStance;

		OnStanceChanged(PreviousStance);
	}
}

void AAlsCharacter::OnStanceChanged_Implementation(EAlsStance PreviousStance) {}

void AAlsCharacter::SetDesiredGait(const EAlsGait NewGait)
{
	if (DesiredGait != NewGait)
	{
		DesiredGait = NewGait;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, DesiredGait, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetDesiredGait(NewGait);
		}
	}
}

void AAlsCharacter::ServerSetDesiredGait_Implementation(const EAlsGait NewGait)
{
	SetDesiredGait(NewGait);
}

void AAlsCharacter::SetGait(const EAlsGait NewGait)
{
	if (Gait != NewGait)
	{
		const auto PreviousGait{Gait};

		Gait = NewGait;

		OnGaitChanged(PreviousGait);
	}
}

void AAlsCharacter::OnGaitChanged_Implementation(EAlsGait PreviousGait) {}

void AAlsCharacter::RefreshGait()
{
	const auto MaxAllowedGait{CalculateMaxAllowedGait()};

	// Update the character max walk speed to the configured speeds based on the currently max allowed gait.

	AlsCharacterMovement->SetMaxAllowedGait(MaxAllowedGait);

	SetGait(CalculateActualGait(MaxAllowedGait));
}

EAlsGait AAlsCharacter::CalculateMaxAllowedGait() const
{
	// Calculate the max allowed gait. This represents the maximum gait the character is currently allowed
	// to be in and can be determined by the desired gait, the rotation mode, the stance, etc. For example,
	// if you wanted to force the character into a walking state while indoors, this could be done here.

	if (Stance == EAlsStance::Standing && (RotationMode != EAlsRotationMode::Aiming || bSprintHasPriorityOverAiming))
	{
		if (DesiredGait == EAlsGait::Sprinting)
		{
			return CanSprint() ? EAlsGait::Sprinting : EAlsGait::Running;
		}

		return DesiredGait;
	}

	// Crouching stance & aiming rotation mode has same behaviour.

	if (DesiredGait == EAlsGait::Sprinting)
	{
		return EAlsGait::Running;
	}

	return DesiredGait;
}

EAlsGait AAlsCharacter::CalculateActualGait(const EAlsGait MaxAllowedGait) const
{
	// Get the new gait. This is calculated by the actual movement of the character and so it can be
	// different from the desired gait or max allowed gait. For instance, if the max allowed gait becomes
	// walking, the new gait will still be running until the character decelerates to the walking speed.

	if (LocomotionState.Speed > AlsCharacterMovement->GetGaitSettings().RunSpeed + 10.0f)
	{
		return MaxAllowedGait == EAlsGait::Sprinting
			       ? EAlsGait::Sprinting
			       : EAlsGait::Running;
	}

	return LocomotionState.Speed >= AlsCharacterMovement->GetGaitSettings().WalkSpeed + 10.0f
		       ? EAlsGait::Running
		       : EAlsGait::Walking;
}

bool AAlsCharacter::CanSprint() const
{
	// Determine if the character is currently able to sprint based on the rotation mode and input
	// rotation. If the character is in the looking direction rotation mode, only allow sprinting
	// if there is input and it is faced forward relative to the camera + or - 50 degrees.

	return LocomotionState.bHasInput &&
	       (RotationMode != EAlsRotationMode::Aiming || bSprintHasPriorityOverAiming) &&
	       // ReSharper disable once CppRedundantParentheses
	       ((ViewMode != EAlsViewMode::FirstPerson &&
	         (DesiredRotationMode == EAlsRotationMode::VelocityDirection || bRotateToVelocityWhenSprinting)) ||
	        FMath::Abs(FRotator::NormalizeAxis(LocomotionState.InputYawAngle - ViewState.SmoothRotation.Yaw)) < 50.0f);
}

void AAlsCharacter::SetDesiredAiming(const bool bNewDesiredAiming)
{
	if (bDesiredAiming != bNewDesiredAiming)
	{
		bDesiredAiming = bNewDesiredAiming;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bDesiredAiming, this)

		OnDesiredAimingChanged(!bNewDesiredAiming);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetDesiredAiming(bNewDesiredAiming);
		}
	}
}

void AAlsCharacter::OnReplicate_DesiredAiming(const bool bPreviousDesiredAiming)
{
	OnDesiredAimingChanged(bPreviousDesiredAiming);
}

void AAlsCharacter::OnDesiredAimingChanged_Implementation(const bool bPreviousDesiredAiming) {}

void AAlsCharacter::ServerSetDesiredAiming_Implementation(const bool bNewAiming)
{
	SetDesiredAiming(bNewAiming);
}

void AAlsCharacter::SetDesiredRotationMode(const EAlsRotationMode NewMode)
{
	if (DesiredRotationMode != NewMode)
	{
		DesiredRotationMode = NewMode;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, DesiredRotationMode, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetDesiredRotationMode(NewMode);
		}
	}
}

void AAlsCharacter::ServerSetDesiredRotationMode_Implementation(const EAlsRotationMode NewMode)
{
	SetDesiredRotationMode(NewMode);
}

void AAlsCharacter::SetRotationMode(const EAlsRotationMode NewMode)
{
	AlsCharacterMovement->SetRotationMode(NewMode);

	if (RotationMode != NewMode)
	{
		const auto PreviousMode{RotationMode};

		RotationMode = NewMode;

		OnRotationModeChanged(PreviousMode);
	}
}

void AAlsCharacter::OnRotationModeChanged_Implementation(EAlsRotationMode PreviousMode) {}

void AAlsCharacter::RefreshRotationMode()
{
	if (ViewMode == EAlsViewMode::FirstPerson)
	{
		const auto bSprinting{Gait == EAlsGait::Sprinting};

		if ((bDesiredAiming || DesiredRotationMode == EAlsRotationMode::Aiming) &&
		    (!bSprinting || !bSprintHasPriorityOverAiming))
		{
			SetRotationMode(!bAllowAimingWhenInAir && LocomotionMode == EAlsLocomotionMode::InAir
				                ? EAlsRotationMode::LookingDirection
				                : EAlsRotationMode::Aiming);
			return;
		}

		// ReSharper disable once CppRedundantParentheses
		if ((bSprinting && DesiredRotationMode == EAlsRotationMode::Aiming) ||
		    DesiredRotationMode == EAlsRotationMode::VelocityDirection)
		{
			SetRotationMode(EAlsRotationMode::LookingDirection);
			return;
		}

		SetRotationMode(DesiredRotationMode);
		return;
	}

	const auto bSprinting{Gait == EAlsGait::Sprinting};

	if ((bDesiredAiming || DesiredRotationMode == EAlsRotationMode::Aiming) &&
	    (!bSprinting || !bSprintHasPriorityOverAiming))
	{
		SetRotationMode(!bAllowAimingWhenInAir && LocomotionMode == EAlsLocomotionMode::InAir
			                ? EAlsRotationMode::LookingDirection
			                : EAlsRotationMode::Aiming);
		return;
	}

	if (bSprinting)
	{
		if (bRotateToVelocityWhenSprinting)
		{
			SetRotationMode(EAlsRotationMode::VelocityDirection);
			return;
		}

		if (DesiredRotationMode == EAlsRotationMode::Aiming)
		{
			SetRotationMode(EAlsRotationMode::LookingDirection);
			return;
		}
	}

	SetRotationMode(DesiredRotationMode);
}

void AAlsCharacter::SetViewMode(const EAlsViewMode NewMode)
{
	if (ViewMode != NewMode)
	{
		ViewMode = NewMode;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ViewMode, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetViewMode(NewMode);
		}
	}
}

void AAlsCharacter::ServerSetViewMode_Implementation(const EAlsViewMode NewMode)
{
	SetViewMode(NewMode);
}

void AAlsCharacter::SetOverlayMode(const FGameplayTag& NewModeTag)
{
	if (OverlayMode != NewModeTag)
	{
		const auto PreviousMode{OverlayMode};

		OverlayMode = NewModeTag;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, OverlayMode, this)

		OnOverlayModeChanged(PreviousMode);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetOverlayMode(NewModeTag);
		}
	}
}

void AAlsCharacter::ServerSetOverlayMode_Implementation(const FGameplayTag& NewMode)
{
	SetOverlayMode(NewMode);
}

void AAlsCharacter::OnReplicate_OverlayMode(const FGameplayTag& PreviousModeTag)
{
	OnOverlayModeChanged(PreviousModeTag);
}

void AAlsCharacter::OnOverlayModeChanged_Implementation(const FGameplayTag& PreviousMode) {}

void AAlsCharacter::SetLocomotionMode(const EAlsLocomotionMode NewMode)
{
	if (LocomotionMode != NewMode)
	{
		const auto PreviousMode{LocomotionMode};

		LocomotionMode = NewMode;

		NotifyLocomotionModeChanged(PreviousMode);
	}
}

void AAlsCharacter::NotifyLocomotionModeChanged(const EAlsLocomotionMode PreviousMode)
{
	ApplyDesiredStance();

	if (PreviousMode == EAlsLocomotionMode::Mantling)
	{
		// Restore default network smoothing mode.

		GetCharacterMovement()->NetworkSmoothingMode = MantlingState.PreviousNetworkSmoothingMode;
	}

	if (LocomotionMode == EAlsLocomotionMode::Grounded)
	{
		if (!LocomotionState.bRotationLocked)
		{
			RefreshTargetYawAngle(LocomotionState.Rotation.Yaw, false);
		}
	}
	else if (LocomotionMode == EAlsLocomotionMode::InAir)
	{
		if (LocomotionAction == EAlsLocomotionAction::None)
		{
			// If the character enters the air, set the in air rotation.

			if (!LocomotionState.bRotationLocked)
			{
				if (InAirRotationMode == EAlsInAirRotationMode::RotateToVelocityOnJump && LocomotionState.bMoving)
				{
					RefreshTargetYawAngle(LocomotionState.VelocityYawAngle, false);
				}
				else if (InAirRotationMode == EAlsInAirRotationMode::KeepWorldRotation)
				{
					RefreshTargetYawAngle(LocomotionState.Rotation.Yaw, false);
				}
			}
		}
		else if (LocomotionAction == EAlsLocomotionAction::Rolling && RollingSettings.bInterruptRollingWhenInAir)
		{
			// If the character is currently rolling, enable the ragdolling.

			StartRagdolling();
		}
	}

	OnLocomotionModeChanged(PreviousMode);
}

void AAlsCharacter::OnLocomotionModeChanged_Implementation(EAlsLocomotionMode PreviousMode) {}

void AAlsCharacter::SetLocomotionAction(const EAlsLocomotionAction NewAction)
{
	if (LocomotionAction != NewAction)
	{
		const auto PreviousAction{LocomotionAction};

		LocomotionAction = NewAction;

		NotifyLocomotionActionChanged(PreviousAction);
	}
}

void AAlsCharacter::NotifyLocomotionActionChanged(const EAlsLocomotionAction PreviousAction)
{
	ApplyDesiredStance();

	OnLocomotionActionChanged(PreviousAction);
}

void AAlsCharacter::OnLocomotionActionChanged_Implementation(EAlsLocomotionAction PreviousAction) {}

void AAlsCharacter::SetInputDirection(FVector NewInputDirection)
{
	NewInputDirection = NewInputDirection.GetSafeNormal();

	if (InputDirection != NewInputDirection)
	{
		InputDirection = NewInputDirection;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InputDirection, this)
	}
}

FTransform AAlsCharacter::CalculateNetworkSmoothedTransform() const
{
	// If network smoothing is disabled, then return regular actor transform.

	return AlsCharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Disabled
		       ? GetActorTransform()
		       : GetActorTransform() * FTransform{
			         GetMesh()->GetRelativeRotationCache().RotatorToQuat(GetMesh()->GetRelativeRotation()) *
			         GetBaseRotationOffset().Inverse(),
			         GetMesh()->GetRelativeLocation() - GetBaseTranslationOffset()
		         };
}

void AAlsCharacter::RefreshLocomotionLocationAndRotation()
{
	const auto& ActorTransform{GetActorTransform()};

	// If network smoothing is disabled, then return regular actor transform.

	if (AlsCharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Disabled)
	{
		LocomotionState.Location = ActorTransform.GetLocation();
		LocomotionState.Rotation = ActorTransform.GetRotation().Rotator();
		return;
	}

	const auto SmoothTransform{
		ActorTransform * FTransform{
			GetMesh()->GetRelativeRotationCache().RotatorToQuat(GetMesh()->GetRelativeRotation()) * GetBaseRotationOffset().Inverse(),
			GetMesh()->GetRelativeLocation() - GetBaseTranslationOffset()
		}
	};

	LocomotionState.Location = SmoothTransform.GetLocation();
	LocomotionState.Rotation = SmoothTransform.GetRotation().Rotator();
}

void AAlsCharacter::RefreshLocomotion(const float DeltaTime)
{
	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		SetInputDirection(AlsCharacterMovement->GetCurrentAcceleration() / AlsCharacterMovement->GetMaxAcceleration());
	}

	// If the character has input, update the input yaw angle.

	LocomotionState.bHasInput = InputDirection.SizeSquared() > KINDA_SMALL_NUMBER;

	if (LocomotionState.bHasInput)
	{
		LocomotionState.InputYawAngle = UAlsMath::DirectionToAngle2D(InputDirection);
	}

	LocomotionState.Velocity = GetVelocity();

	// Determine if the character is moving by getting it's speed. The speed equals the length
	// of the horizontal velocity, so it does not take vertical movement into account. If the
	// character is moving, update the last velocity rotation. This value is saved because it
	// might be useful to know the last orientation of movement even after the character has stopped.

	LocomotionState.Speed = LocomotionState.Velocity.Size2D();
	LocomotionState.bHasSpeed = LocomotionState.Speed > 1.0f;

	if (LocomotionState.bHasSpeed)
	{
		LocomotionState.VelocityYawAngle = UAlsMath::DirectionToAngle2D(LocomotionState.Velocity);
	}

	// Character is moving if has speed and current acceleration, or if the speed is greater than moving speed threshold.

	LocomotionState.bMoving = LocomotionState.bHasInput || LocomotionState.Speed > MovingSpeedThreshold;

	const auto NewAcceleration{(LocomotionState.Velocity - LocomotionState.PreviousVelocity) / DeltaTime};
	LocomotionState.Acceleration = NewAcceleration.SizeSquared() > SMALL_NUMBER || IsLocallyControlled()
		                               ? NewAcceleration
		                               : LocomotionState.Acceleration * 0.5f;

	RefreshLocomotionLocationAndRotation();
}

FRotator AAlsCharacter::GetViewRotation() const
{
	return ViewRotation;
}

void AAlsCharacter::SetViewRotation(const FRotator& NewViewRotation)
{
	if (ViewRotation != NewViewRotation)
	{
		ViewRotation = NewViewRotation;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ViewRotation, this)

		if (!IsReplicatingMovement() && GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetViewRotation(NewViewRotation);
		}
	}
}

void AAlsCharacter::ServerSetViewRotation_Implementation(const FRotator& NewViewRotation)
{
	SetViewRotation(NewViewRotation);
}

void AAlsCharacter::RefreshView(const float DeltaTime)
{
	if (IsReplicatingMovement() && GetLocalRole() >= ROLE_AutonomousProxy || IsLocallyControlled())
	{
		SetViewRotation(Super::GetViewRotation().GetNormalized());
	}

	// Interpolate view rotation to current control rotation for smooth character
	// rotation movement. Decrease interpolation speed for slower but smoother movement.

	ViewState.SmoothRotation = UAlsMath::ExponentialDecay(ViewState.SmoothRotation, ViewRotation, DeltaTime, 30.0f);
	ViewState.SmoothRotation.Normalize();

	// Set the yaw speed by comparing the current and previous view yaw angle, divided
	// by delta seconds. This represents the speed the camera is rotating left to right.

	ViewState.YawSpeed = FMath::Abs((ViewState.SmoothRotation.Yaw - ViewState.PreviousSmoothYawAngle) / DeltaTime);
}

void AAlsCharacter::RefreshGroundedActorRotation(const float DeltaTime)
{
	if (LocomotionState.bRotationLocked)
	{
		return;
	}

	if (LocomotionAction == EAlsLocomotionAction::Rolling)
	{
		// Rolling.

		if (RollingSettings.bRotateToInputDuringRoll && LocomotionState.bHasInput)
		{
			RefreshTargetYawAngle(UAlsMath::InterpolateAngleConstant(LocomotionState.TargetYawAngle,
			                                                         LocomotionState.InputYawAngle,
			                                                         DeltaTime, RollingSettings.InputInterpolationSpeed));
		}

		RefreshActorRotation(LocomotionState.TargetYawAngle, DeltaTime, RollingSettings.ActorRotationInterpolationSpeed);
		return;
	}

	if (LocomotionAction != EAlsLocomotionAction::None || HasAnyRootMotion())
	{
		// Other actions are ignored.
		return;
	}

	auto* AnimationInstance{CastChecked<UAlsAnimationInstance>(GetMesh()->GetAnimInstance())};

	if (LocomotionState.bMoving)
	{
		// Moving.

		if (TryRefreshCustomGroundedMovingActorRotation(DeltaTime))
		{
			return;
		}

		switch (RotationMode)
		{
			case EAlsRotationMode::VelocityDirection:
				RefreshActorRotationExtraSmooth(LocomotionState.VelocityYawAngle, DeltaTime, CalculateActorRotationSpeed(),
				                                800.0f);
				break;

			case EAlsRotationMode::LookingDirection:
			{
				const auto TargetYawAngle{
					Gait == EAlsGait::Sprinting
						? LocomotionState.VelocityYawAngle
						: FRotator::NormalizeAxis(ViewState.SmoothRotation.Yaw +
						                          AnimationInstance->GetCurveValue(UAlsConstants::RotationYawOffsetCurve()))
				};

				RefreshActorRotationExtraSmooth(TargetYawAngle, DeltaTime, CalculateActorRotationSpeed(), 500.0f);
			}
			break;

			case EAlsRotationMode::Aiming:
				RefreshGroundedMovingAimingActorRotation(DeltaTime);
				break;
		}

		return;
	}

	// Not moving.

	// ReSharper disable once CppRedundantParentheses
	if ((!TryRefreshCustomGroundedNotMovingActorRotation(DeltaTime) &&
	     RotationMode == EAlsRotationMode::Aiming) || ViewMode == EAlsViewMode::FirstPerson)
	{
		RefreshGroundedNotMovingAimingActorRotation(DeltaTime);
	}

	if (AnimationInstance->IsRotationYawSpeedAppliedThisFrame())
	{
		// Skip actor rotation modification using the rotation yaw speed animation curve because animation
		// blueprint has not been updated yet (animation blueprint has a lower update rate than character actor).
		return;
	}

	AnimationInstance->SetRotationYawSpeedAppliedThisFrame(true);

	const auto RotationYawSpeed{AnimationInstance->GetCurveValue(UAlsConstants::RotationYawSpeedCurve())};
	if (FMath::Abs(RotationYawSpeed) > KINDA_SMALL_NUMBER)
	{
		// Apply the rotation yaw speed curve from animations.

		AddActorWorldRotation({0.0f, RotationYawSpeed * DeltaTime, 0.0f});

		RefreshLocomotionLocationAndRotation();

		RefreshTargetYawAngle(LocomotionState.Rotation.Yaw);
	}

	if (GetLocalRole() >= ROLE_Authority && !IsNetMode(NM_Standalone))
	{
		// Fully update the animation blueprint on the server when not moving to keep the rotation in sync between clients and the server.

		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
}

bool AAlsCharacter::TryRefreshCustomGroundedMovingActorRotation(const float DeltaTime)
{
	return false;
}

bool AAlsCharacter::TryRefreshCustomGroundedNotMovingActorRotation(const float DeltaTime)
{
	return false;
}

void AAlsCharacter::RefreshGroundedMovingAimingActorRotation(const float DeltaTime)
{
	RefreshActorRotationExtraSmooth(ViewState.SmoothRotation.Yaw, DeltaTime, 20.0f, 1000.0f);
}

void AAlsCharacter::RefreshGroundedNotMovingAimingActorRotation(const float DeltaTime)
{
	if (LocomotionState.bHasInput)
	{
		RefreshActorRotationExtraSmooth(ViewState.SmoothRotation.Yaw, DeltaTime, 20.0f, 1000.0f);
		return;
	}

	// Prevent the character from rotating past a certain angle.

	const auto YawAngleDifference{FRotator::NormalizeAxis(ViewState.SmoothRotation.Yaw - LocomotionState.Rotation.Yaw)};
	if (FMath::Abs(YawAngleDifference) > 70.0f)
	{
		RefreshActorRotation(FRotator::NormalizeAxis(ViewState.SmoothRotation.Yaw + (YawAngleDifference > 0.0f ? -70.0f : 70.0f)),
		                     DeltaTime, 20.0f);
	}
}

float AAlsCharacter::CalculateActorRotationSpeed() const
{
	// Calculate the rotation speed by using the rotation speed curve in the movement gait settings. Using
	// the curve in conjunction with the gait amount gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	return AlsCharacterMovement->GetGaitSettings().RotationSpeedCurve->GetFloatValue(AlsCharacterMovement->CalculateGaitAmount()) *
	       FMath::GetMappedRangeValueClamped({0.0f, 300.0f}, {1.0f, 3.0f}, ViewState.YawSpeed);
}

void AAlsCharacter::RefreshInAirActorRotation(const float DeltaTime)
{
	if (LocomotionState.bRotationLocked)
	{
		return;
	}

	if (LocomotionAction == EAlsLocomotionAction::Rolling)
	{
		// Rolling.

		if (RollingSettings.bRotateToInputDuringRoll && LocomotionState.bHasInput)
		{
			RefreshTargetYawAngle(UAlsMath::InterpolateAngleConstant(LocomotionState.TargetYawAngle,
			                                                         LocomotionState.InputYawAngle,
			                                                         DeltaTime, RollingSettings.InputInterpolationSpeed));
		}

		RefreshActorRotation(LocomotionState.TargetYawAngle, DeltaTime, RollingSettings.ActorRotationInterpolationSpeed);

		return;
	}

	if (LocomotionAction != EAlsLocomotionAction::None)
	{
		// Other actions are ignored.
		return;
	}

	if (TryRefreshCustomInAirActorRotation(DeltaTime))
	{
		return;
	}

	switch (RotationMode)
	{
		case EAlsRotationMode::VelocityDirection:
		case EAlsRotationMode::LookingDirection:
			if (InAirRotationMode == EAlsInAirRotationMode::RotateToVelocityOnJump && LocomotionState.bMoving)
			{
				RefreshActorRotation(LocomotionState.VelocityYawAngle, DeltaTime, 5.0f);
			}
			else if (InAirRotationMode == EAlsInAirRotationMode::KeepRelativeRotation)
			{
				RefreshActorRotation(ViewState.SmoothRotation.Yaw - LocomotionState.ViewRelativeTargetYawAngle, DeltaTime, 5.0f);
			}
			break;

		case EAlsRotationMode::Aiming:
			RefreshInAirAimingActorRotation(DeltaTime);
			break;
	}
}

bool AAlsCharacter::TryRefreshCustomInAirActorRotation(const float DeltaTime)
{
	return false;
}

void AAlsCharacter::RefreshInAirAimingActorRotation(const float DeltaTime)
{
	RefreshActorRotation(ViewState.SmoothRotation.Yaw, DeltaTime, 15.0f);
}

void AAlsCharacter::RefreshTargetYawAngle(const float TargetYawAngle, const bool bRefreshSmoothTargetYawAngle)
{
	LocomotionState.TargetYawAngle = TargetYawAngle;

	if (bRefreshSmoothTargetYawAngle)
	{
		LocomotionState.SmoothTargetYawAngle = TargetYawAngle;
	}

	LocomotionState.ViewRelativeTargetYawAngle = FRotator::NormalizeAxis(ViewState.SmoothRotation.Yaw - TargetYawAngle);
}

void AAlsCharacter::RefreshActorRotationInstant(const float TargetYawAngle, const ETeleportType Teleport)
{
	RefreshTargetYawAngle(TargetYawAngle);

	SetActorRotation({0.0f, TargetYawAngle, 0.0f}, Teleport);

	RefreshLocomotionLocationAndRotation();
}

void AAlsCharacter::RefreshActorRotation(const float TargetYawAngle, const float DeltaTime, const float ActorRotationSpeed)
{
	RefreshTargetYawAngle(TargetYawAngle);

	SetActorRotation(UAlsMath::ExponentialDecay(GetActorRotation(), {0.0f, TargetYawAngle, 0.0f}, DeltaTime, ActorRotationSpeed));

	RefreshLocomotionLocationAndRotation();
}

void AAlsCharacter::RefreshActorRotationExtraSmooth(const float TargetYawAngle, const float DeltaTime,
                                                    const float ActorRotationSpeed, const float TargetYawAngleRotationSpeed)
{
	RefreshTargetYawAngle(TargetYawAngle, false);

	// Interpolate the target rotation for extra smooth rotation behavior.

	LocomotionState.SmoothTargetYawAngle = UAlsMath::InterpolateAngleConstant(LocomotionState.SmoothTargetYawAngle,
	                                                                          TargetYawAngle, DeltaTime, TargetYawAngleRotationSpeed);

	SetActorRotation(UAlsMath::ExponentialDecay(GetActorRotation(), {0.0f, LocomotionState.SmoothTargetYawAngle, 0.0f},
	                                            DeltaTime, ActorRotationSpeed));

	RefreshLocomotionLocationAndRotation();
}

void AAlsCharacter::LockRotation(const float TargetYawAngle)
{
	if (LocomotionState.bRotationLocked)
	{
		UE_LOG(LogAls, Warning, TEXT("%s: Trying to lock a rotation when it is already locked!"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	MulticastLockRotation(TargetYawAngle);
}

void AAlsCharacter::UnLockRotation()
{
	if (!LocomotionState.bRotationLocked)
	{
		UE_LOG(LogAls, Log, TEXT("%s: Trying to unlock a rotation when it is already unlocked!"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	MulticastUnLockRotation();
}

void AAlsCharacter::MulticastLockRotation_Implementation(const float TargetYawAngle)
{
	LocomotionState.bRotationLocked = true;

	RefreshActorRotationInstant(TargetYawAngle, ETeleportType::TeleportPhysics);
}

void AAlsCharacter::MulticastUnLockRotation_Implementation()
{
	LocomotionState.bRotationLocked = false;
}

void AAlsCharacter::MulticastOnJumpedNetworked_Implementation()
{
	if (!IsLocallyControlled())
	{
		OnJumpedNetworked();
	}
}

void AAlsCharacter::OnJumpedNetworked()
{
	CastChecked<UAlsAnimationInstance>(GetMesh()->GetAnimInstance())->Jump();
}

void AAlsCharacter::MulticastOnLandedNetworked_Implementation()
{
	if (!IsLocallyControlled())
	{
		OnLandedNetworked();
	}
}

void AAlsCharacter::OnLandedNetworked()
{
	const auto VerticalSpeed{FMath::Abs(AlsCharacterMovement->Velocity.Z)};

	if (RagdollingSettings.bStartRagdollingOnLand && VerticalSpeed > RagdollingSettings.RagdollingOnLandSpeedThreshold)
	{
		StartRagdolling();
		return;
	}

	if (RollingSettings.bStartRollingOnLand && VerticalSpeed >= RollingSettings.RollingOnLandSpeedThreshold)
	{
		StartRolling(1.3f, LocomotionState.bHasSpeed
			                   ? LocomotionState.VelocityYawAngle
			                   : LocomotionState.Rotation.Yaw);
		return;
	}

	AlsCharacterMovement->BrakingFrictionFactor = LocomotionState.bHasInput ? 0.5f : 3.0f;

	GetWorldTimerManager().SetTimer(LandedGroundFrictionResetTimer, this, &ThisClass::OnLandedGroundFrictionReset, 0.5f, false);
}

void AAlsCharacter::OnLandedGroundFrictionReset() const
{
	AlsCharacterMovement->BrakingFrictionFactor = 0.0f;
}
