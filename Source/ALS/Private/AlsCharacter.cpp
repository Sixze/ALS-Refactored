#include "AlsCharacter.h"

#include "AlsAnimationInstance.h"
#include "AlsCharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Settings/AlsCharacterSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsLog.h"
#include "Utility/AlsMath.h"
#include "Utility/GameplayTags/AlsLocomotionActionTags.h"

AAlsCharacter::AAlsCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAlsCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	GetCapsuleComponent()->InitCapsuleSize(30.0f, 90.0f);

	GetMesh()->SetRelativeLocation_Direct({0.0f, 0.0f, -90.0f});
	GetMesh()->SetRelativeRotation_Direct({0.0f, -90.0f, 0.0f});
	GetMesh()->SetAnimInstanceClass(UAlsAnimationInstance::StaticClass());

	// Required for the flail animation to work correctly when ragdolling.

	GetMesh()->bUpdateJointsFromAnimation = true;

	// If this option is enabled, then it can cause problems with animation curves (for example, aiming will not work correctly).
	// https://answers.unrealengine.com/questions/1001006/view.html

	GetMesh()->bEnableUpdateRateOptimizations = false;

	AlsCharacterMovement = Cast<UAlsCharacterMovementComponent>(GetCharacterMovement());
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
	check(IsValid(Settings))
	check(IsValid(MovementSettings))

	Super::PostInitializeComponents();

	// Make sure that the pose is always ticked on the server when the character is controlled
	// by a remote client, otherwise some problems may arise (such as jitter when rolling).

	GetMesh()->VisibilityBasedAnimTickOption =
		!IsNetMode(NM_Standalone) && GetLocalRole() >= ROLE_Authority && GetRemoteRole() == ROLE_AutonomousProxy
			? EVisibilityBasedAnimTickOption::AlwaysTickPose
			: EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;

	// Make sure the mesh and animation blueprint update after the character to ensure it gets the most recent values.

	GetMesh()->AddTickPrerequisiteActor(this);

	AlsCharacterMovement->OnPhysicsRotation.AddUObject(this, &ThisClass::PhysicsRotation);

	// Pass current movement settings to the movement component.

	AlsCharacterMovement->SetMovementSettings(MovementSettings);

	AlsAnimationInstance = CastChecked<UAlsAnimationInstance>(GetMesh()->GetAnimInstance());

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
	// Restore visibility based animation tick option.
	// Make sure that the pose is always ticked on the server when the character is controlled
	// by a remote client, otherwise some problems may arise (such as jitter when rolling).

	GetMesh()->VisibilityBasedAnimTickOption =
		!IsNetMode(NM_Standalone) && GetLocalRole() >= ROLE_Authority && GetRemoteRole() == ROLE_AutonomousProxy
			? EVisibilityBasedAnimTickOption::AlwaysTickPose
			: EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;

	if (!GetMesh()->bRecentlyRendered && GetMesh()->VisibilityBasedAnimTickOption > EVisibilityBasedAnimTickOption::AlwaysTickPose)
	{
		AlsAnimationInstance->SetPendingUpdate(true);
	}

	RefreshRotationMode();

	RefreshLocomotion(DeltaTime);
	RefreshView(DeltaTime);

	RefreshGait();

	RefreshGroundedActorRotation(DeltaTime);
	RefreshInAirActorRotation(DeltaTime);

	TryStartMantlingInAir();

	RefreshMantling();
	RefreshRagdolling(DeltaTime);

	LocomotionState.PreviousVelocity = LocomotionState.Velocity;
	ViewState.PreviousSmoothYawAngle = ViewState.SmoothRotation.Yaw;

	Super::Tick(DeltaTime);

	AlsAnimationInstance->SetRecentlyUpdated(false);
}

void AAlsCharacter::Jump()
{
	if (LocomotionMode == EAlsLocomotionMode::Grounded &&
	    !LocomotionAction.IsValid() && Stance == EAlsStance::Standing)
	{
		Super::Jump();
	}
}

void AAlsCharacter::OnMovementModeChanged(const EMovementMode PreviousMode, const uint8 PreviousCustomMode)
{
	// Use the character movement mode to set the locomotion mode to the right value. This allows you to have a
	// custom set of movement modes but still use the functionality of the default character movement component.

	switch (GetCharacterMovement()->MovementMode)
	{
		case MOVE_Walking:
		case MOVE_NavWalking:
			SetLocomotionMode(EAlsLocomotionMode::Grounded);
			break;

		case MOVE_Falling:
			SetLocomotionMode(EAlsLocomotionMode::InAir);
			break;

		default:
			SetLocomotionMode(EAlsLocomotionMode::None);
			break;
	}

	Super::OnMovementModeChanged(PreviousMode, PreviousCustomMode);
}

void AAlsCharacter::OnStartCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (LocomotionAction == FAlsLocomotionActionTags::Get().Rolling)
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

void AAlsCharacter::PhysicsRotation(const float DeltaTime)
{
	RefreshRollingPhysics(DeltaTime);
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
	if (!LocomotionAction.IsValid())
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
	else if (LocomotionAction == FAlsLocomotionActionTags::Get().Rolling && Settings->Rolling.bCrouchOnStart)
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
	if (LocomotionMode != EAlsLocomotionMode::Grounded)
	{
		return;
	}

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

	if (Stance == EAlsStance::Standing && (RotationMode != EAlsRotationMode::Aiming || Settings->bSprintHasPriorityOverAiming))
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
	       (RotationMode != EAlsRotationMode::Aiming || Settings->bSprintHasPriorityOverAiming) &&
	       // ReSharper disable once CppRedundantParentheses
	       ((ViewMode != EAlsViewMode::FirstPerson &&
	         (DesiredRotationMode == EAlsRotationMode::VelocityDirection || Settings->bRotateToVelocityWhenSprinting)) ||
	        FMath::Abs(FRotator::NormalizeAxis(LocomotionState.InputYawAngle - ViewRotation.Yaw)) < 50.0f);
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
		    (!bSprinting || !Settings->bSprintHasPriorityOverAiming))
		{
			SetRotationMode(!Settings->bAllowAimingWhenInAir && LocomotionMode == EAlsLocomotionMode::InAir
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
	    (!bSprinting || !Settings->bSprintHasPriorityOverAiming))
	{
		SetRotationMode(!Settings->bAllowAimingWhenInAir && LocomotionMode == EAlsLocomotionMode::InAir
			                ? EAlsRotationMode::LookingDirection
			                : EAlsRotationMode::Aiming);
		return;
	}

	if (bSprinting)
	{
		if (Settings->bRotateToVelocityWhenSprinting)
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

void AAlsCharacter::ServerSetOverlayMode_Implementation(const FGameplayTag& NewModeTag)
{
	SetOverlayMode(NewModeTag);
}

void AAlsCharacter::OnReplicate_OverlayMode(const FGameplayTag& PreviousModeTag)
{
	OnOverlayModeChanged(PreviousModeTag);
}

void AAlsCharacter::OnOverlayModeChanged_Implementation(const FGameplayTag& PreviousModeTag) {}

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

	if (LocomotionMode == EAlsLocomotionMode::Grounded)
	{
		if (PreviousMode == EAlsLocomotionMode::InAir)
		{
			if (Settings->Ragdolling.bStartRagdollingOnLand &&
			    LocomotionState.Velocity.Z <= -Settings->Ragdolling.RagdollingOnLandSpeedThreshold)
			{
				StartRagdolling();
			}
			else if (Settings->Rolling.bStartRollingOnLand &&
			         LocomotionState.Velocity.Z <= -Settings->Rolling.RollingOnLandSpeedThreshold)
			{
				StartRolling(1.3f, LocomotionState.bHasSpeed
					                   ? LocomotionState.VelocityYawAngle
					                   : GetActorRotation().Yaw);
			}
			else
			{
				GetCharacterMovement()->BrakingFrictionFactor = LocomotionState.bHasInput ? 0.5f : 3.0f;

				GetWorldTimerManager().SetTimer(LandedGroundFrictionResetTimer, this, &ThisClass::OnLandedGroundFrictionReset, 0.5f, false);
			}
		}
		else if (!LocomotionState.bRotationLocked)
		{
			RefreshTargetYawAngle(LocomotionState.Rotation.Yaw, false);
		}
	}
	else if (LocomotionMode == EAlsLocomotionMode::InAir)
	{
		if (!LocomotionAction.IsValid())
		{
			// If the character enters the air, set the in air rotation.

			if (!LocomotionState.bRotationLocked)
			{
				if (Settings->InAirRotationMode == EAlsInAirRotationMode::RotateToVelocityOnJump && LocomotionState.bMoving)
				{
					RefreshTargetYawAngle(LocomotionState.VelocityYawAngle, false);
				}
				else if (Settings->InAirRotationMode == EAlsInAirRotationMode::KeepWorldRotation)
				{
					RefreshTargetYawAngle(LocomotionState.Rotation.Yaw, false);
				}
			}
		}
		else if (LocomotionAction == FAlsLocomotionActionTags::Get().Rolling && Settings->Rolling.bInterruptRollingWhenInAir)
		{
			// If the character is currently rolling, enable the ragdolling.

			StartRagdolling();
		}
	}

	OnLocomotionModeChanged(PreviousMode);
}

void AAlsCharacter::OnLocomotionModeChanged_Implementation(EAlsLocomotionMode PreviousMode) {}

void AAlsCharacter::SetLocomotionAction(const FGameplayTag& NewActionTag)
{
	if (LocomotionAction != NewActionTag)
	{
		const auto PreviousAction{LocomotionAction};

		LocomotionAction = NewActionTag;

		NotifyLocomotionActionChanged(PreviousAction);
	}
}

void AAlsCharacter::NotifyLocomotionActionChanged(const FGameplayTag& PreviousActionTag)
{
	ApplyDesiredStance();

	OnLocomotionActionChanged(PreviousActionTag);
}

void AAlsCharacter::OnLocomotionActionChanged_Implementation(const FGameplayTag& PreviousActionTag) {}

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

	return GetCharacterMovement()->NetworkSmoothingMode == ENetworkSmoothingMode::Disabled
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

	if (GetCharacterMovement()->NetworkSmoothingMode == ENetworkSmoothingMode::Disabled)
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
		SetInputDirection(GetCharacterMovement()->GetCurrentAcceleration() / GetCharacterMovement()->GetMaxAcceleration());
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

	LocomotionState.bMoving = LocomotionState.bHasInput || LocomotionState.Speed > Settings->MovingSpeedThreshold;

	LocomotionState.Acceleration = (LocomotionState.Velocity - LocomotionState.PreviousVelocity) / DeltaTime;

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
	// ReSharper disable once CppRedundantParentheses
	if ((IsReplicatingMovement() && GetLocalRole() >= ROLE_AutonomousProxy) || IsLocallyControlled())
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
	if (LocomotionState.bRotationLocked || LocomotionMode != EAlsLocomotionMode::Grounded ||
	    LocomotionAction.IsValid() || HasAnyRootMotion())
	{
		ResetRotationYawSpeed();
		return;
	}

	if (!LocomotionState.bMoving)
	{
		// Not moving.

		// ReSharper disable once CppRedundantParentheses
		if ((!TryRefreshCustomGroundedNotMovingActorRotation(DeltaTime) &&
		     RotationMode == EAlsRotationMode::Aiming) || ViewMode == EAlsViewMode::FirstPerson)
		{
			RefreshGroundedNotMovingAimingActorRotation(DeltaTime);
		}

		ApplyRotationYawSpeed(DeltaTime);
		return;
	}

	// Moving.

	ResetRotationYawSpeed();

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
					                          GetMesh()->GetAnimInstance()->GetCurveValue(UAlsConstants::RotationYawOffsetCurve()))
			};

			RefreshActorRotationExtraSmooth(TargetYawAngle, DeltaTime, CalculateActorRotationSpeed(), 500.0f);
		}
		break;

		case EAlsRotationMode::Aiming:
			RefreshGroundedMovingAimingActorRotation(DeltaTime);
			break;
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

void AAlsCharacter::ApplyRotationYawSpeed(const float DeltaTime)
{
	if (!AlsAnimationInstance->IsRecentlyUpdated())
	{
		// Skip actor rotation modification because animation blueprint has not been updated
		// yet (probably animation blueprint has a lower tick rate than the character).
		return;
	}

	const auto AdjustedDeltaTime{FMath::Min(DeltaTime, 1.0f / 30.0f)};

	const auto Time{GetWorld()->GetTimeSeconds()};
	const auto SkippedTime{Time - LocomotionState.LastRotationYawSpeedApplyTime - AdjustedDeltaTime};

	const auto RotationYawSpeed{GetMesh()->GetAnimInstance()->GetCurveValue(UAlsConstants::RotationYawSpeedCurve())};
	auto DeltaYawAngle{RotationYawSpeed * AdjustedDeltaTime};

	if (SkippedTime > SMALL_NUMBER)
	{
		// Try to guess how much the character should rotate if it hasn't been updated for a long time, and rotate
		// by this amount. This is necessary so that the character always rotates approximately by the same angle,
		// regardless of the frame rate or the difference in the tick rate of the animation blueprint and the character.

		DeltaYawAngle += FMath::Lerp(LocomotionState.LastRotationYawSpeed, RotationYawSpeed, 0.5f) * SkippedTime;
	}

	if (FMath::Abs(DeltaYawAngle) > KINDA_SMALL_NUMBER)
	{
		AddActorWorldRotation({0.0f, DeltaYawAngle, 0.0f});

		RefreshLocomotionLocationAndRotation();

		RefreshTargetYawAngle(LocomotionState.Rotation.Yaw);
	}

	LocomotionState.LastRotationYawSpeedApplyTime = Time;
	LocomotionState.LastRotationYawSpeed = RotationYawSpeed;
}

void AAlsCharacter::ResetRotationYawSpeed()
{
	LocomotionState.LastRotationYawSpeedApplyTime = GetWorld()->GetTimeSeconds();
	LocomotionState.LastRotationYawSpeed = 0.0f;
}

void AAlsCharacter::RefreshInAirActorRotation(const float DeltaTime)
{
	if (LocomotionState.bRotationLocked || LocomotionMode != EAlsLocomotionMode::InAir ||
	    LocomotionAction.IsValid() || TryRefreshCustomInAirActorRotation(DeltaTime))
	{
		return;
	}

	switch (RotationMode)
	{
		case EAlsRotationMode::VelocityDirection:
		case EAlsRotationMode::LookingDirection:
			if (Settings->InAirRotationMode == EAlsInAirRotationMode::RotateToVelocityOnJump && LocomotionState.bMoving)
			{
				RefreshActorRotation(LocomotionState.VelocityYawAngle, DeltaTime, 5.0f);
			}
			else if (Settings->InAirRotationMode == EAlsInAirRotationMode::KeepRelativeRotation)
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

	auto NewRotation{GetActorRotation()};
	NewRotation.Yaw = TargetYawAngle;

	SetActorRotation(NewRotation, Teleport);

	RefreshLocomotionLocationAndRotation();
}

void AAlsCharacter::RefreshActorRotation(const float TargetYawAngle, const float DeltaTime, const float ActorRotationInterpolationSpeed)
{
	RefreshTargetYawAngle(TargetYawAngle);

	auto NewRotation{GetActorRotation()};
	NewRotation.Yaw = UAlsMath::ExponentialDecayAngle(NewRotation.Yaw, TargetYawAngle, DeltaTime, ActorRotationInterpolationSpeed);

	SetActorRotation(NewRotation);

	RefreshLocomotionLocationAndRotation();
}

void AAlsCharacter::RefreshActorRotationExtraSmooth(const float TargetYawAngle, const float DeltaTime,
                                                    const float ActorRotationInterpolationSpeed,
                                                    const float TargetYawAngleRotationSpeed)
{
	RefreshTargetYawAngle(TargetYawAngle, false);

	// Interpolate the target rotation for extra smooth rotation behavior.

	LocomotionState.SmoothTargetYawAngle = UAlsMath::InterpolateAngleConstant(LocomotionState.SmoothTargetYawAngle,
	                                                                          TargetYawAngle, DeltaTime, TargetYawAngleRotationSpeed);

	auto NewRotation{GetActorRotation()};
	NewRotation.Yaw = UAlsMath::ExponentialDecayAngle(NewRotation.Yaw, LocomotionState.SmoothTargetYawAngle,
	                                                  DeltaTime, ActorRotationInterpolationSpeed);

	SetActorRotation(NewRotation);

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
	AlsAnimationInstance->Jump();
}

void AAlsCharacter::OnLandedGroundFrictionReset() const
{
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
}
