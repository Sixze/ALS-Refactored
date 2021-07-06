#include "AlsCharacter.h"

#include "AlsAnimationInstance.h"
#include "AlsCharacterMovementComponent.h"
#include "DisplayDebugHelpers.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utility/AlsEnumerationUtility.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

const FCollisionObjectQueryParams AAlsCharacter::MantlingAndRagdollObjectQueryParameters{
	ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_Destructible)
};

AAlsCharacter::AAlsCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAlsCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	GetMesh()->SetRelativeLocation_Direct({0.0f, 0.0f, -90.0f});
	GetMesh()->SetRelativeRotation_Direct({0.0f, -90.0f, 0.0f});

	// Required for the flail animation to work correctly when ragdolling.

	GetMesh()->bUpdateJointsFromAnimation = true;

	// Uncomment the line of code below if you have noticeable foot lock issues on simulated proxies.

	// GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// If this option is enabled, then it can cause problems with animation curves (for example, aiming will not work correctly).
	// https://answers.unrealengine.com/questions/1001006/view.html

	GetMesh()->bEnableUpdateRateOptimizations = false;

	AlsCharacterMovement = Cast<UAlsCharacterMovementComponent>(GetCharacterMovement());

	MantlingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MantlingTimeline"));
	MantlingTimeline->SetLooping(false);
	MantlingTimeline->SetTimelineLengthMode(TL_TimelineLength);
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
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AimingRotation, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RagdollTargetLocation, Parameters)
}

void AAlsCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Make sure the mesh and animation blueprint update after the character to ensure it gets the most recent values.

	GetMesh()->AddTickPrerequisiteActor(this);

	if (GetLocalRole() <= ROLE_SimulatedProxy)
	{
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	// Pass current movement settings to the movement component.

	AlsCharacterMovement->SetMovementSettings(MovementSettings);

	FOnTimelineFloat TimelineUpdated{};
	TimelineUpdated.BindDynamic(this, &ThisClass::OnMantlingTimelineUpdated);
	MantlingTimeline->AddInterpFloat(GeneralMantlingSettings.TimelineCurve, TimelineUpdated);

	FOnTimelineEvent TimelineFinished{};
	TimelineFinished.BindDynamic(this, &ThisClass::OnMantlingTimelineEnded);
	MantlingTimeline->SetTimelineFinishedFunc(TimelineFinished);

	// Set default rotation values.

	RefreshSmoothLocationAndRotation();

	LocomotionState.InputYawAngle = LocomotionState.SmoothRotation.Yaw;
	LocomotionState.VelocityYawAngle = LocomotionState.SmoothRotation.Yaw;
	LocomotionState.TargetActorRotation = LocomotionState.SmoothRotation;
	LocomotionState.PreviousSmoothRotation = LocomotionState.SmoothRotation;

	AimingState.SmoothRotation = AimingRotation;

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
	Super::Tick(DeltaTime);

	RefreshRotationMode();

	RefreshLocomotion(DeltaTime);
	RefreshAiming(DeltaTime);

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

		case EAlsLocomotionMode::Ragdolling:
			RefreshRagdolling(DeltaTime);
			break;
	}

	LocomotionState.PreviousVelocity = LocomotionState.Velocity;
	AimingState.PreviousSmoothYawAngle = AimingState.SmoothRotation.Yaw;
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
	if (LocomotionMode == EAlsLocomotionMode::Grounded &&
	    LocomotionAction == EAlsLocomotionAction::None)
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
}

void AAlsCharacter::SetStance(const EAlsStance NewStance)
{
	if (Stance != NewStance)
	{
		const auto PreviousStance{Stance};

		Stance = NewStance;

		AlsCharacterMovement->SetStance(Stance);

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
	// Calculate the allowed gait. This represents the maximum gait the character is currently allowed to
	// be in, and can be determined by the desired gait, the rotation mode, the stance, etc. For example,
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
	// Get the new gait. This is calculated by the actual movement of the character, and so it can be
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
	       (ViewMode != EAlsViewMode::FirstPerson &&
	        (DesiredRotationMode == EAlsRotationMode::VelocityDirection || bRotateToVelocityWhenSprinting) ||
	        FMath::Abs(FRotator::NormalizeAxis(LocomotionState.InputYawAngle - AimingState.SmoothRotation.Yaw)) < 50.0f);
}

void AAlsCharacter::SetDesiredAiming(const bool bNewDesiredAiming)
{
	if (bDesiredAiming != bNewDesiredAiming)
	{
		bDesiredAiming = bNewDesiredAiming;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bDesiredAiming, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetDesiredAiming(bNewDesiredAiming);
		}
	}
}

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
	if (RotationMode != NewMode)
	{
		const auto PreviousMode{RotationMode};

		RotationMode = NewMode;

		AlsCharacterMovement->SetRotationMode(RotationMode);

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

		if (bSprinting && DesiredRotationMode == EAlsRotationMode::Aiming ||
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

void AAlsCharacter::SetOverlayMode(const EAlsOverlayMode NewMode)
{
	if (OverlayMode != NewMode)
	{
		const auto PreviousMode{OverlayMode};

		OverlayMode = NewMode;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, OverlayMode, this)

		OnOverlayModeChanged(PreviousMode);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetOverlayMode(NewMode);
		}
	}
}

void AAlsCharacter::ServerSetOverlayMode_Implementation(const EAlsOverlayMode NewMode)
{
	SetOverlayMode(NewMode);
}

void AAlsCharacter::OnReplicate_OverlayMode(const EAlsOverlayMode PreviousMode)
{
	OnOverlayModeChanged(PreviousMode);
}

void AAlsCharacter::OnOverlayModeChanged_Implementation(EAlsOverlayMode PreviousMode) {}

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
	if (LocomotionMode == EAlsLocomotionMode::Grounded)
	{
		ApplyDesiredStance();
	}
	else if (LocomotionMode == EAlsLocomotionMode::Ragdolling && PreviousMode == EAlsLocomotionMode::Mantling)
	{
		// Stop the mantling timeline if transitioning to the ragdolling mode while mantling.

		MantlingTimeline->Stop();
	}
	else if (LocomotionMode == EAlsLocomotionMode::InAir)
	{
		if (LocomotionAction == EAlsLocomotionAction::None)
		{
			// If the character enters the air, set the in air rotation and un crouch if crouched.

			if (InAirRotationMode == EAlsInAirRotationMode::KeepLookingDirectionRelativeRotation)
			{
				InAirState.TargetYawAngle = FRotator::NormalizeAxis(AimingState.SmoothRotation.Yaw - LocomotionState.SmoothRotation.Yaw);
				InAirState.bLookingDirectionRelativeTargetYawAngle = true;
			}
			else
			{
				InAirState.TargetYawAngle = LocomotionState.SmoothRotation.Yaw;
				InAirState.bLookingDirectionRelativeTargetYawAngle = false;
			}

			if (Stance == EAlsStance::Crouching)
			{
				UnCrouch();
			}
		}
		else if (LocomotionAction == EAlsLocomotionAction::Rolling)
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
	if (LocomotionAction == EAlsLocomotionAction::Rolling && RollingSettings.bCrouchOnStart)
	{
		// Make the character crouch if performing a roll.

		Crouch();
	}

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

FTransform AAlsCharacter::CalculateSmoothTransform() const
{
	// If network smoothing is disabled, then return regular actor transform.

	return AlsCharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Disabled
		       ? GetActorTransform()
		       : GetActorTransform() * FTransform{
			         GetMesh()->GetRelativeRotationCache().RotatorToQuat(GetMesh()->GetRelativeRotation()) * BaseRotationOffset.Inverse(),
			         GetMesh()->GetRelativeLocation() - BaseTranslationOffset
		         };
}

void AAlsCharacter::RefreshSmoothLocationAndRotation()
{
	const auto SmoothTransform{CalculateSmoothTransform()};

	LocomotionState.SmoothLocation = SmoothTransform.GetLocation();
	LocomotionState.SmoothRotation = SmoothTransform.GetRotation().Rotator();
}

void AAlsCharacter::RefreshLocomotion(const float DeltaTime)
{
	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		SetInputDirection(AlsCharacterMovement->GetCurrentAcceleration() / AlsCharacterMovement->GetMaxAcceleration());
	}

	// If the character has input, update the input yaw angle.

	LocomotionState.bHasInput = InputDirection.SizeSquared() > SMALL_NUMBER;

	if (LocomotionState.bHasInput)
	{
		LocomotionState.InputYawAngle = UAlsMath::DirectionToAngle(FVector2D{InputDirection});
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
		LocomotionState.VelocityYawAngle = UAlsMath::DirectionToAngle(FVector2D{LocomotionState.Velocity});
	}

	// Character is moving if has speed and current acceleration, or if the speed is greater than moving speed threshold.

	LocomotionState.bMoving = LocomotionState.bHasInput || LocomotionState.Speed > MovingSpeedThreshold;

	const auto NewAcceleration{(LocomotionState.Velocity - LocomotionState.PreviousVelocity) / DeltaTime};
	LocomotionState.Acceleration = NewAcceleration.SizeSquared() > SMALL_NUMBER || IsLocallyControlled()
		                               ? NewAcceleration
		                               : LocomotionState.Acceleration * 0.5f;

	LocomotionState.PreviousSmoothRotation = LocomotionState.SmoothRotation;

	RefreshSmoothLocationAndRotation();
}

void AAlsCharacter::SetAimingRotation(const FRotator& NewAimingRotation)
{
	if (AimingRotation != NewAimingRotation)
	{
		AimingRotation = NewAimingRotation;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, AimingRotation, this)
	}
}

void AAlsCharacter::RefreshAiming(const float DeltaTime)
{
	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		SetAimingRotation(GetControlRotation().GetNormalized());
	}

	// Interpolate aiming rotation to current control rotation for smooth character
	// rotation movement. Decrease interpolation speed for slower but smoother movement.

	AimingState.SmoothRotation = FMath::RInterpTo(AimingState.SmoothRotation, AimingRotation, DeltaTime, 30.0f);

	// Set the yaw speed by comparing the current and previous aiming yaw angle, divided
	// by delta seconds. This represents the speed the camera is rotating left to right.

	AimingState.YawSpeed = FMath::Abs((AimingState.SmoothRotation.Yaw - AimingState.PreviousSmoothYawAngle) / DeltaTime);
}

void AAlsCharacter::RefreshGroundedActorRotation(const float DeltaTime)
{
	if (LocomotionAction == EAlsLocomotionAction::Rolling)
	{
		// Rolling.

		if (RollingSettings.bRotateToInputDuringRoll && LocomotionState.bHasInput)
		{
			RollingState.TargetYawAngle = UAlsMath::InterpolateAngleConstant(RollingState.TargetYawAngle, LocomotionState.InputYawAngle,
			                                                                 DeltaTime, RollingSettings.InputInterpolationSpeed);
		}

		RefreshActorRotation(RollingState.TargetYawAngle, DeltaTime, RollingSettings.ActorRotationInterpolationSpeed);
		return;
	}

	if (LocomotionAction != EAlsLocomotionAction::None)
	{
		// Other actions are ignored.

		return;
	}

	if (LocomotionState.bMoving && !HasAnyRootMotion())
	{
		// Moving.

		switch (RotationMode)
		{
			case EAlsRotationMode::VelocityDirection:
				RefreshActorRotationExtraSmooth(LocomotionState.VelocityYawAngle, DeltaTime, 800.0f,
				                                CalculateActorRotationSpeed());
				break;

			case EAlsRotationMode::LookingDirection:
			{
				const auto TargetYawAngle{
					Gait == EAlsGait::Sprinting
						? LocomotionState.VelocityYawAngle
						: AimingState.SmoothRotation.Yaw + GetMesh()->GetAnimInstance()->GetCurveValue(
							  UAlsConstants::RotationYawOffsetCurve())
				};

				RefreshActorRotationExtraSmooth(TargetYawAngle, DeltaTime, 500.0f, CalculateActorRotationSpeed());
			}
			break;

			case EAlsRotationMode::Aiming:
				RefreshActorRotationExtraSmooth(AimingState.SmoothRotation.Yaw, DeltaTime, 1000.0f, 20.0f);
				break;
		}

		return;
	}

	// Not moving.

	if (RotationMode == EAlsRotationMode::Aiming && ViewMode != EAlsViewMode::FirstPerson || ViewMode == EAlsViewMode::FirstPerson)
	{
		if (LocomotionState.bHasInput)
		{
			RefreshActorRotationExtraSmooth(AimingState.SmoothRotation.Yaw, DeltaTime, 1000.0f, 20.0f);
		}
		else
		{
			RefreshAimingActorRotation(DeltaTime);
		}
	}

	const auto RotationYawSpeed{GetMesh()->GetAnimInstance()->GetCurveValue(UAlsConstants::RotationYawSpeedCurve())};
	if (FMath::Abs(RotationYawSpeed) <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// Apply the rotation yaw speed curve from animations.

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		LocomotionState.TargetActorRotation.Yaw = FRotator::NormalizeAxis(
			LocomotionState.TargetActorRotation.Yaw + RotationYawSpeed * DeltaTime
		);

		SetActorRotation(LocomotionState.TargetActorRotation);
	}
	else
	{
		AddActorWorldRotation({0.0f, RotationYawSpeed * DeltaTime, 0.0f});
	}

	RefreshSmoothLocationAndRotation();

	LocomotionState.TargetActorRotation = LocomotionState.SmoothRotation;
}

void AAlsCharacter::RefreshAimingActorRotation(const float DeltaTime)
{
	// Prevent the character from rotating past a certain angle.

	const auto YawAngleDifference{FRotator::NormalizeAxis(AimingState.SmoothRotation.Yaw - LocomotionState.SmoothRotation.Yaw)};

	if (FMath::Abs(YawAngleDifference) > 70.0f)
	{
		RefreshActorRotation(AimingState.SmoothRotation.Yaw + (YawAngleDifference > 0.0f ? -70.0f : 70.0f),
		                     DeltaTime, 20.0f);
	}
}

void AAlsCharacter::RefreshInAirActorRotation(const float DeltaTime)
{
	if (LocomotionAction == EAlsLocomotionAction::Rolling)
	{
		// Rolling.

		RefreshActorRotation(RollingState.TargetYawAngle, DeltaTime, RollingSettings.ActorRotationInterpolationSpeed);
		return;
	}

	switch (RotationMode)
	{
		case EAlsRotationMode::VelocityDirection:
		case EAlsRotationMode::LookingDirection:
			RefreshActorRotation(InAirState.bLookingDirectionRelativeTargetYawAngle
				                     ? AimingState.SmoothRotation.Yaw - InAirState.TargetYawAngle
				                     : InAirState.TargetYawAngle, DeltaTime, 5.0f);
			break;

		case EAlsRotationMode::Aiming:
			RefreshActorRotation(AimingState.SmoothRotation.Yaw, DeltaTime, 15.0f);

			InAirState.TargetYawAngle = InAirState.bLookingDirectionRelativeTargetYawAngle
				                            ? FRotator::NormalizeAxis(AimingState.SmoothRotation.Yaw - LocomotionState.SmoothRotation.Yaw)
				                            : LocomotionState.SmoothRotation.Yaw;
			break;
	}
}

float AAlsCharacter::CalculateActorRotationSpeed() const
{
	// Calculate the rotation speed by using the rotation speed curve in the movement gait settings. Using
	// the curve in conjunction with the gait amount gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	return AlsCharacterMovement->GetGaitSettings().RotationSpeedCurve->GetFloatValue(AlsCharacterMovement->CalculateGaitAmount()) *
	       FMath::GetMappedRangeValueClamped({0.0f, 300.0f}, {1.0f, 3.0f}, AimingState.YawSpeed);
}

void AAlsCharacter::RefreshActorRotation(const float TargetYawAngle, const float DeltaTime, const float RotationSpeed)
{
	LocomotionState.TargetActorRotation = {0.0f, TargetYawAngle, 0.0f};

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), LocomotionState.TargetActorRotation, DeltaTime, RotationSpeed));

	RefreshSmoothLocationAndRotation();
}

void AAlsCharacter::RefreshActorRotationExtraSmooth(const float TargetYawAngle, const float DeltaTime,
                                                    const float TargetRotationSpeed, const float ActorRotationSpeed)
{
	// Interpolate the target actor rotation for extra smooth rotation behavior.

	LocomotionState.TargetActorRotation = FMath::RInterpConstantTo(LocomotionState.TargetActorRotation,
	                                                               {0.0f, TargetYawAngle, 0.0f},
	                                                               DeltaTime, TargetRotationSpeed);

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), LocomotionState.TargetActorRotation, DeltaTime, ActorRotationSpeed));

	RefreshSmoothLocationAndRotation();
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
	if (InAirRotationMode == EAlsInAirRotationMode::RotateToVelocityOnJump && LocomotionState.Speed > 100.0f)
	{
		InAirState.TargetYawAngle = LocomotionState.VelocityYawAngle;
		InAirState.bLookingDirectionRelativeTargetYawAngle = false;
	}
	else if (InAirRotationMode == EAlsInAirRotationMode::KeepLookingDirectionRelativeRotation)
	{
		InAirState.TargetYawAngle = FRotator::NormalizeAxis(AimingState.SmoothRotation.Yaw - LocomotionState.SmoothRotation.Yaw);
		InAirState.bLookingDirectionRelativeTargetYawAngle = true;
	}
	else
	{
		InAirState.TargetYawAngle = LocomotionState.SmoothRotation.Yaw;
		InAirState.bLookingDirectionRelativeTargetYawAngle = false;
	}

	Cast<UAlsAnimationInstance>(GetMesh()->GetAnimInstance())->Jump();
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
			                   : LocomotionState.SmoothRotation.Yaw);
		return;
	}

	AlsCharacterMovement->BrakingFrictionFactor = LocomotionState.bHasInput ? 0.5f : 3.0f;

	GetWorldTimerManager().SetTimer(LandedGroundFrictionResetTimer, this, &ThisClass::OnLandedGroundFrictionReset, 0.5f, false);
}

void AAlsCharacter::OnLandedGroundFrictionReset() const
{
	AlsCharacterMovement->BrakingFrictionFactor = 0.0f;
}

bool AAlsCharacter::TryStartMantlingGrounded()
{
	return LocomotionMode == EAlsLocomotionMode::Grounded &&
	       LocomotionAction == EAlsLocomotionAction::None &&
	       TryStartMantling(GeneralMantlingSettings.GroundedTrace);
}

bool AAlsCharacter::TryStartMantlingInAir()
{
	return TryStartMantling(GeneralMantlingSettings.InAirTrace);
}

bool AAlsCharacter::TryStartMantling(const FAlsMantlingTraceSettings& TraceSettings)
{
	if (LocomotionMode == EAlsLocomotionMode::Mantling)
	{
		return false;
	}

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebug(this, UAlsUtility::MantlingDisplayName())};
#endif

	const auto* Capsule{GetCapsuleComponent()};

	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	const auto CapsuleBottomLocation{
		LocomotionState.SmoothLocation - LocomotionState.SmoothRotation.RotateVector(FVector::UpVector) * CapsuleHalfHeight
	};

	const auto TraceCapsuleRadius{CapsuleRadius - 1.0f};

	const auto LedgeHeightDelta{TraceSettings.LedgeHeight.GetMax() - TraceSettings.LedgeHeight.GetMin()};

	// Trace forward to find a object the character cannot walk on.

	static const FName ForwardTraceTagName{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	const FVector ForwardTraceDirection{
		UAlsMath::AngleToDirection(
			LocomotionState.bHasInput
				? LocomotionState.SmoothRotation.Yaw +
				  FMath::ClampAngle(UAlsMath::DirectionToAngle(FVector2D{InputDirection}) - LocomotionState.SmoothRotation.Yaw,
				                    -GeneralMantlingSettings.MaxReachAngle, GeneralMantlingSettings.MaxReachAngle)
				: LocomotionState.bHasSpeed
				? LocomotionState.SmoothRotation.Yaw +
				  FMath::ClampAngle(LocomotionState.VelocityYawAngle - LocomotionState.SmoothRotation.Yaw,
				                    -GeneralMantlingSettings.MaxReachAngle, GeneralMantlingSettings.MaxReachAngle)
				: LocomotionState.SmoothRotation.Yaw),
		0.0f
	};

	auto ForwardTraceStart{CapsuleBottomLocation - ForwardTraceDirection * CapsuleRadius};
	ForwardTraceStart.Z += (TraceSettings.LedgeHeight.X + TraceSettings.LedgeHeight.Y) * 0.5f - UCharacterMovementComponent::MAX_FLOOR_DIST;

	auto ForwardTraceEnd{ForwardTraceStart + ForwardTraceDirection * (CapsuleRadius + TraceSettings.ReachDistance + 1.0f)};

	const auto ForwardTraceCapsuleHalfHeight{LedgeHeightDelta * 0.5f};

	FHitResult ForwardTraceHit;
	GetWorld()->SweepSingleByObjectType(ForwardTraceHit, ForwardTraceStart, ForwardTraceEnd, FQuat::Identity,
	                                    MantlingAndRagdollObjectQueryParameters,
	                                    FCollisionShape::MakeCapsule(TraceCapsuleRadius, ForwardTraceCapsuleHalfHeight),
	                                    {ForwardTraceTagName, false, this});

	auto* TargetPrimitive{ForwardTraceHit.GetComponent()};

	if (!ForwardTraceHit.IsValidBlockingHit() ||
	    !IsValid(TargetPrimitive) ||
	    TargetPrimitive->GetComponentVelocity().SizeSquared() > FMath::Square(GeneralMantlingSettings.TargetPrimitiveSpeedThreshold) ||
	    !ForwardTraceHit.Component->CanCharacterStepUp(this) ||
	    AlsCharacterMovement->IsWalkable(ForwardTraceHit))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, false, ForwardTraceHit, {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);
		}
#endif

		return false;
	}

	// Trace downward from the first trace's impact point and determine if the hit location is walkable.

	static const FName DownwardTraceTagName{FString::Format(TEXT("{0} (Downward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	const auto TargetLocationOffset{FVector2D{ForwardTraceHit.ImpactNormal.GetSafeNormal2D()} * TraceSettings.TargetLocationOffset};

	const FVector DownwardTraceStart{
		ForwardTraceHit.ImpactPoint.X - TargetLocationOffset.X, ForwardTraceHit.ImpactPoint.Y - TargetLocationOffset.Y,
		CapsuleBottomLocation.Z + LedgeHeightDelta + 2.5f * TraceCapsuleRadius + UCharacterMovementComponent::MIN_FLOOR_DIST
	};

	const FVector DownwardTraceEnd{
		DownwardTraceStart.X, DownwardTraceStart.Y,
		CapsuleBottomLocation.Z + TraceSettings.LedgeHeight.GetMin() + TraceCapsuleRadius - UCharacterMovementComponent::MAX_FLOOR_DIST
	};

	FHitResult DownwardTraceHit;
	GetWorld()->SweepSingleByObjectType(DownwardTraceHit, DownwardTraceStart, DownwardTraceEnd, FQuat::Identity,
	                                    MantlingAndRagdollObjectQueryParameters, FCollisionShape::MakeSphere(TraceCapsuleRadius),
	                                    {DownwardTraceTagName, false, this});

	if (!AlsCharacterMovement->IsWalkable(DownwardTraceHit))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit, {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);

			UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd, TraceCapsuleRadius,
			                                        false, DownwardTraceHit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f},
			                                        TraceSettings.bDrawFailedTraces ? 7.5f : 0.0f);
		}
#endif

		return false;
	}

	// Check if the capsule has room to stand at the downward trace's location. If so,
	// set that location as the target transform and calculate the mantling height.

	static const FName FreeSpaceTraceTagName{FString::Format(TEXT("{0} (Free Space Overlap)"), {ANSI_TO_TCHAR(__FUNCTION__)})};

	const FVector TargetLocation{
		DownwardTraceHit.ImpactPoint.X, DownwardTraceHit.ImpactPoint.Y,
		DownwardTraceHit.ImpactPoint.Z + CapsuleHalfHeight + UCharacterMovementComponent::MIN_FLOOR_DIST
	};

	if (GetWorld()->OverlapAnyTestByObjectType(TargetLocation, FQuat::Identity, MantlingAndRagdollObjectQueryParameters,
	                                           FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
	                                           {FreeSpaceTraceTagName, false, this}))
	{
#if ENABLE_DRAW_DEBUG
		if (bDisplayDebug)
		{
			UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
			                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit, {0.0f, 0.25f, 1.0f},
			                                                    {0.0f, 0.75f, 1.0f}, TraceSettings.bDrawFailedTraces ? 5.0f : 0.0f);

			UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd, TraceCapsuleRadius,
			                                        false, DownwardTraceHit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f},
			                                        TraceSettings.bDrawFailedTraces ? 7.5f : 0.0f);

			DrawDebugCapsule(Capsule->GetWorld(), TargetLocation, CapsuleHalfHeight, CapsuleRadius,
			                 FQuat::Identity, FColor::Red, false, TraceSettings.bDrawFailedTraces ? 10.0f : 0.0f);
		}
#endif

		return false;
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebug)
	{
		UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceCapsuleRadius,
		                                                    ForwardTraceCapsuleHalfHeight, true, ForwardTraceHit,
		                                                    {0.0f, 0.25f, 1.0f}, {0.0f, 0.75f, 1.0f}, 5.0f);

		UAlsUtility::DrawDebugSweepSingleSphere(GetWorld(), DownwardTraceStart, DownwardTraceEnd,
		                                        TraceCapsuleRadius, true, DownwardTraceHit,
		                                        {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f}, 7.5f);
	}
#endif

	const auto TargetRotation{(-ForwardTraceHit.ImpactNormal.GetSafeNormal2D()).ToOrientationRotator()};

	const auto MantlingHeight{TargetLocation.Z - LocomotionState.SmoothLocation.Z};

	// Determine the mantling type by checking the movement mode and mantling height.

	EAlsMantlingType MantlingType;
	if (LocomotionMode == EAlsLocomotionMode::InAir)
	{
		MantlingType = EAlsMantlingType::InAir;
	}
	else
	{
		MantlingType = MantlingHeight > 125.0f
			               ? EAlsMantlingType::High
			               : EAlsMantlingType::Low;
	}

	const FAlsMantlingParameters Parameters{TargetPrimitive, TargetLocation, TargetRotation, MantlingHeight, MantlingType};

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartMantling(Parameters);
	}
	else
	{
		StartMantlingImplementation(Parameters);
		ServerStartMantling(Parameters);
	}

	return true;
}

void AAlsCharacter::ServerStartMantling_Implementation(const FAlsMantlingParameters& Parameters)
{
	MulticastStartMantling(Parameters);

	ForceNetUpdate();
}

void AAlsCharacter::MulticastStartMantling_Implementation(const FAlsMantlingParameters& Parameters)
{
	StartMantlingImplementation(Parameters);
}

void AAlsCharacter::StartMantlingImplementation(const FAlsMantlingParameters& Parameters)
{
	if (LocomotionMode == EAlsLocomotionMode::Mantling)
	{
		return;
	}

	// This will help to get rid of the jitter on the client side due to mispredictions of the character's future position.

	MantlingState.PreviousNetworkSmoothingMode = AlsCharacterMovement->NetworkSmoothingMode;

	AlsCharacterMovement->NetworkSmoothingMode = ENetworkSmoothingMode::Disabled;

	GetMesh()->SetRelativeLocationAndRotation(BaseTranslationOffset, BaseRotationOffset);

	LocomotionState.SmoothLocation = GetActorLocation();
	LocomotionState.SmoothRotation = GetActorRotation();

	// Selects the mantling settings and use it to set the new mantling state.

	const auto MantlingSettings{SelectMantlingSettings(Parameters.MantlingType)};

	MantlingState.Montage = MantlingSettings.Montage;
	MantlingState.InterpolationAndCorrectionCurve = MantlingSettings.InterpolationAndCorrectionAmountsCurve;

	MantlingState.PlayRate = FMath::GetMappedRangeValueClamped(MantlingSettings.ReferenceHeight, MantlingSettings.PlayRate,
	                                                           Parameters.MantlingHeight);

	MantlingState.StartTime = FMath::GetMappedRangeValueClamped(MantlingSettings.ReferenceHeight, MantlingSettings.StartTime,
	                                                            Parameters.MantlingHeight);

	MantlingState.Primitive = Parameters.TargetPrimitive;
	MantlingState.TargetTransform = {Parameters.TargetRotation, Parameters.TargetLocation, FVector::OneVector};

	// Convert the world space target transform to the primitive's local space for use in moving objects.

	MantlingState.RelativeTransform = MantlingState.TargetTransform * Parameters.TargetPrimitive->GetComponentTransform().Inverse();

	// Calculate the actor offset transform (offset amount between the actor and target transform).

	MantlingState.ActorOffset = UAlsMath::SubtractTransforms(CalculateSmoothTransform(), MantlingState.TargetTransform);

	// Calculate the animation offset transform from the target location. This would be
	// the location the actual animation starts at relative to the T target transform.

	auto AnimationOffsetLocation{Parameters.TargetRotation.Vector() * MantlingSettings.StartRelativeLocation.X};
	AnimationOffsetLocation.Z = MantlingSettings.StartRelativeLocation.Z;

	MantlingState.AnimationOffset = AnimationOffsetLocation;

	// Clear the character movement mode and set the movement state to mantling.

	AlsCharacterMovement->SetMovementMode(MOVE_None);
	SetLocomotionMode(EAlsLocomotionMode::Mantling);

	// Configure the mantling timeline so that it is the same length as the interpolation and
	// correction curve minus the starting position, and plays at the same speed as the animation.

	auto MinTime{0.0f};
	auto MaxTime{0.0f};
	MantlingState.InterpolationAndCorrectionCurve->GetTimeRange(MinTime, MaxTime);

	MantlingTimeline->SetTimelineLength(MaxTime - MantlingState.StartTime);
	MantlingTimeline->SetPlayRate(MantlingState.PlayRate);
	MantlingTimeline->PlayFromStart();

	// Play the animation montage if valid.

	if (IsValid(MantlingState.Montage))
	{
		GetMesh()->GetAnimInstance()->Montage_Play(MantlingState.Montage, MantlingState.PlayRate, EMontagePlayReturnType::MontageLength,
		                                           MantlingState.StartTime + MantlingTimeline->GetPlaybackPosition(), false);
	}

	OnMantlingStarted(Parameters);
}

FAlsMantlingSettings AAlsCharacter::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return {};
}

void AAlsCharacter::OnMantlingStarted_Implementation(const FAlsMantlingParameters& Parameters) {}

void AAlsCharacter::OnMantlingTimelineUpdated(float BlendInTime)
{
	// Continually update the target transform from the stored relative transform to follow along with moving objects.

	MantlingState.TargetTransform = MantlingState.RelativeTransform * MantlingState.Primitive->GetComponentTransform();

	// Update the interpolation and correction amounts using the interpolation and correction amounts curve.

	const auto InterpolationAndCorrectionAmounts{
		MantlingState.InterpolationAndCorrectionCurve->GetVectorValue(MantlingState.StartTime + MantlingTimeline->GetPlaybackPosition())
	};

	const auto InterpolationAmount{InterpolationAndCorrectionAmounts.X};
	const auto HorizontalCorrectionAmount{InterpolationAndCorrectionAmounts.Y};
	const auto VerticalCorrectionAmount{InterpolationAndCorrectionAmounts.Z};

	// Lerp multiple transforms together for independent control over the horizontal
	// and vertical blend to the animation offset, as well as the target transform.

	// Blend into the animation horizontal offset.

	const FTransform TargetHorizontalOffset{
		FRotator::ZeroRotator,
		{
			MantlingState.AnimationOffset.X,
			MantlingState.AnimationOffset.Y,
			MantlingState.ActorOffset.GetLocation().Z
		},
		FVector::OneVector
	};

	const auto ResultHorizontalOffset{
		UKismetMathLibrary::TLerp(MantlingState.ActorOffset, TargetHorizontalOffset, HorizontalCorrectionAmount)
	};

	// Blend into the animation vertical offset.

	const FTransform TargetVerticalOffset{
		MantlingState.ActorOffset.GetRotation(),
		{
			MantlingState.ActorOffset.GetLocation().X,
			MantlingState.ActorOffset.GetLocation().Y,
			MantlingState.AnimationOffset.Z
		},
		FVector::OneVector
	};

	const auto ResultVerticalOffset{
		UKismetMathLibrary::TLerp(MantlingState.ActorOffset, TargetVerticalOffset, VerticalCorrectionAmount)
	};

	const FTransform ResultOffset{
		ResultHorizontalOffset.GetRotation(),
		{
			ResultHorizontalOffset.GetLocation().X,
			ResultHorizontalOffset.GetLocation().Y,
			ResultVerticalOffset.GetLocation().Z
		},
		FVector::OneVector
	};

	// Blend from the current blending transforms into the final transform.

	const auto TargetTransform{
		UKismetMathLibrary::TLerp(UAlsMath::AddTransforms(MantlingState.TargetTransform, ResultOffset), MantlingState.TargetTransform,
		                          InterpolationAmount)
	};

	// Initial blend in (controlled in the timeline curve) to allow the actor to blend into the interpolation and
	// correction curve at the midpoint. This prevents pops when mantling an object lower than the animated mantling.

	const auto ResultTransform{
		UKismetMathLibrary::TLerp(UAlsMath::AddTransforms(MantlingState.TargetTransform, MantlingState.ActorOffset), TargetTransform,
		                          BlendInTime)
	};

	SetActorLocationAndRotation(ResultTransform.GetLocation(), ResultTransform.GetRotation());

	GetMesh()->SetRelativeLocationAndRotation(BaseTranslationOffset, BaseRotationOffset);

	RefreshSmoothLocationAndRotation();

	LocomotionState.TargetActorRotation = LocomotionState.SmoothRotation;
}

void AAlsCharacter::OnMantlingTimelineEnded()
{
	// Restore default network smoothing mode.

	AlsCharacterMovement->NetworkSmoothingMode = MantlingState.PreviousNetworkSmoothingMode;

	// Set the character movement mode to walking.

	AlsCharacterMovement->SetMovementMode(MOVE_Walking);

	OnMantlingEnded();
}

void AAlsCharacter::OnMantlingEnded_Implementation() {}

void AAlsCharacter::StartRagdolling()
{
	if (GetLocomotionMode() == EAlsLocomotionMode::Ragdolling)
	{
		return;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartRagdolling();
	}
	else
	{
		ServerStartRagdolling();
	}
}

void AAlsCharacter::ServerStartRagdolling_Implementation()
{
	MulticastStartRagdolling();
}

void AAlsCharacter::MulticastStartRagdolling_Implementation()
{
	StartRagdollingImplementation();
}

void AAlsCharacter::StartRagdollingImplementation()
{
	if (GetLocomotionMode() == EAlsLocomotionMode::Ragdolling)
	{
		return;
	}

	// When networked, disable replicate movement reset ragdolling target location and pull force variables
	// and if the host is a dedicated server, change animation tick option to avoid z-location bug.

	SetReplicateMovement(false);
	AlsCharacterMovement->bIgnoreClientMovementErrorChecksAndCorrection = true;

	if (GetLocalRole() >= ROLE_AutonomousProxy)
	{
		SetRagdollTargetLocation(GetMesh()->GetSocketLocation(UAlsConstants::PelvisBone()));
	}

	RagdollingState.PullForce = 0.0f;

	if (IsNetMode(NM_DedicatedServer))
	{
		RagdollingState.PreviousVisibilityBasedAnimTickOption = GetMesh()->VisibilityBasedAnimTickOption;
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}

	// Clear the character movement mode and set the movement state to ragdolling.

	AlsCharacterMovement->SetMovementMode(MOVE_None);
	SetLocomotionMode(EAlsLocomotionMode::Ragdolling);

	// Disable capsule collision and enable mesh physics simulation starting from the pelvis.

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(UAlsConstants::PelvisBone(), true, true);

	// Stop any active montages.

	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);

	OnRagdollingStarted();
}

void AAlsCharacter::OnRagdollingStarted_Implementation() {}

void AAlsCharacter::SetRagdollTargetLocation(const FVector& NewLocation)
{
	if (RagdollTargetLocation != NewLocation)
	{
		RagdollTargetLocation = NewLocation;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, RagdollTargetLocation, this)

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetRagdollTargetLocation(NewLocation);
		}
	}
}

void AAlsCharacter::ServerSetRagdollTargetLocation_Implementation(const FVector& NewTargetLocation)
{
	SetRagdollTargetLocation(NewTargetLocation);
}

void AAlsCharacter::RefreshRagdolling(const float DeltaTime)
{
	const auto RootBoneVelocity{GetMesh()->GetPhysicsLinearVelocity(UAlsConstants::RootBone())};

	RagdollingState.RootBoneVelocity = RootBoneVelocity.SizeSquared() > SMALL_NUMBER || IsLocallyControlled()
		                                   ? RootBoneVelocity
		                                   : RagdollingState.RootBoneVelocity * 0.5f;

	// Use the velocity to scale the ragdoll's joint strength for physical animation.

	GetMesh()->SetAllMotorsAngularDriveParams(UAlsMath::Clamp01(RagdollingState.RootBoneVelocity.Size() / 1000.0f) * 25000.0f,
	                                          0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual
	// acceleration. This also prevents the ragdoll from going through the floor.

	GetMesh()->SetEnableGravity(RagdollingState.RootBoneVelocity.Z > -4000.0f);

	RefreshRagdollingActorTransform(DeltaTime);
}

void AAlsCharacter::RefreshRagdollingActorTransform(float DeltaTime)
{
	const auto PelvisTransform{GetMesh()->GetSocketTransform(UAlsConstants::PelvisBone())};

	if (IsLocallyControlled())
	{
		SetRagdollTargetLocation(PelvisTransform.GetLocation());
	}

	// Trace downward from the target location to offset the target location, preventing the lower
	// half of the capsule from going through the floor when the ragdoll is laying on the ground.

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, RagdollTargetLocation, {
		                                        RagdollTargetLocation.X,
		                                        RagdollTargetLocation.Y,
		                                        RagdollTargetLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
	                                        }, MantlingAndRagdollObjectQueryParameters,
	                                        {__FUNCTION__, false, this});

	auto NewActorLocation{RagdollTargetLocation};

	RagdollingState.bGrounded = Hit.IsValidBlockingHit();

	if (RagdollingState.bGrounded)
	{
		NewActorLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - FMath::Abs(Hit.ImpactPoint.Z - Hit.TraceStart.Z) + 2.0f;
	}

	if (!IsLocallyControlled())
	{
		RagdollingState.PullForce = FMath::FInterpTo(RagdollingState.PullForce, 750.0f, DeltaTime, 0.6f);

		const auto RagdollSpeedSquared{FVector2D{RagdollingState.RootBoneVelocity.X, RagdollingState.RootBoneVelocity.Y}.SizeSquared()};

		const auto PullForceSocketName{
			RagdollSpeedSquared > FMath::Square(300.0f)
				? UAlsConstants::Spine03Bone()
				: UAlsConstants::PelvisBone()
		};

		GetMesh()->AddForce((RagdollTargetLocation - GetMesh()->GetSocketLocation(PullForceSocketName)) * RagdollingState.PullForce,
		                    PullForceSocketName, true);
	}

	// Determine whether the ragdoll is facing upward or downward and set the target rotation accordingly.

	const auto PelvisRotation{PelvisTransform.Rotator()};

	RagdollingState.bFacedUpward = PelvisRotation.Roll < 0.0f;

	SetActorLocationAndRotation(NewActorLocation, {
		                            0.0f, RagdollingState.bFacedUpward ? PelvisRotation.Yaw - 180.0f : PelvisRotation.Yaw, 0.0f
	                            });

	RefreshSmoothLocationAndRotation();

	LocomotionState.TargetActorRotation = LocomotionState.SmoothRotation;
}

bool AAlsCharacter::TryStopRagdolling()
{
	if (GetLocomotionMode() != EAlsLocomotionMode::Ragdolling)
	{
		return false;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStopRagdolling();
	}
	else
	{
		ServerStopRagdolling();
	}

	return true;
}

void AAlsCharacter::ServerStopRagdolling_Implementation()
{
	MulticastStopRagdolling();

	ForceNetUpdate();
}

void AAlsCharacter::MulticastStopRagdolling_Implementation()
{
	StopRagdollingImplementation();
}

void AAlsCharacter::StopRagdollingImplementation()
{
	if (GetLocomotionMode() != EAlsLocomotionMode::Ragdolling)
	{
		return;
	}

	// Re-enable replicate movement and if the host is a dedicated server set animation tick option back to default.

	SetReplicateMovement(true);
	AlsCharacterMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;

	if (IsNetMode(NM_DedicatedServer))
	{
		GetMesh()->VisibilityBasedAnimTickOption = RagdollingState.PreviousVisibilityBasedAnimTickOption;
	}

	// If the ragdoll is on the ground, set the movement mode to walking and play a get up animation. If not, set
	// the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.

	if (RagdollingState.bGrounded)
	{
		AlsCharacterMovement->SetMovementMode(MOVE_Walking);
		GetMesh()->GetAnimInstance()->Montage_Play(SelectGetUpMontage(RagdollingState.bFacedUpward), 1.0f,
		                                           EMontagePlayReturnType::MontageLength, 0.0f, true);
	}
	else
	{
		AlsCharacterMovement->SetMovementMode(MOVE_Falling);
		AlsCharacterMovement->Velocity = RagdollingState.RootBoneVelocity;
	}

	// Re-enable capsule collision, and disable physics simulation on the mesh.

	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	OnRagdollingEnded();
}

UAnimMontage* AAlsCharacter::SelectGetUpMontage_Implementation(bool bRagdollFacedUpward)
{
	return nullptr;
}

void AAlsCharacter::OnRagdollingEnded_Implementation() {}

void AAlsCharacter::TryStartRolling(const float PlayRate)
{
	if (LocomotionMode == EAlsLocomotionMode::Grounded &&
	    LocomotionAction == EAlsLocomotionAction::None)
	{
		StartRolling(PlayRate, LocomotionState.bHasInput &&
		                       (RollingSettings.bRotateToInputOnStart ||
		                        RollingSettings.bRotateToInputDuringRoll && RollingSettings.InputInterpolationSpeed <= 0.0f)
			                       ? LocomotionState.InputYawAngle
			                       : LocomotionState.SmoothRotation.Yaw);
	}
}

void AAlsCharacter::StartRolling(const float PlayRate, const float TargetYawAngle)
{
	auto* Montage{SelectRollMontage()};

	if (!IsValid(Montage))
	{
		return;
	}

	if (GetLocalRole() >= ROLE_Authority)
	{
		MulticastStartRolling(Montage, PlayRate, TargetYawAngle);
	}
	else
	{
		StartRollingImplementation(Montage, PlayRate, TargetYawAngle);

		ServerStartRolling(Montage, PlayRate, TargetYawAngle);
	}
}

UAnimMontage* AAlsCharacter::SelectRollMontage_Implementation()
{
	return nullptr;
}

void AAlsCharacter::ServerStartRolling_Implementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	MulticastStartRolling(Montage, PlayRate, TargetYawAngle);

	ForceNetUpdate();
}

void AAlsCharacter::MulticastStartRolling_Implementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	StartRollingImplementation(Montage, PlayRate, TargetYawAngle);
}

void AAlsCharacter::StartRollingImplementation(UAnimMontage* Montage, const float PlayRate, const float TargetYawAngle)
{
	RollingState.TargetYawAngle = TargetYawAngle;

	if (RollingSettings.ActorRotationInterpolationSpeed <= 0.0f)
	{
		LocomotionState.TargetActorRotation = {0.0f, TargetYawAngle, 0.0f};

		SetActorRotation(LocomotionState.TargetActorRotation, ETeleportType::TeleportPhysics);

		RefreshSmoothLocationAndRotation();
	}

	if (IsValid(Montage) && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(Montage))
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}
}

void AAlsCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalPosition)
{
	const auto Scale{FMath::Min(Canvas->SizeX / (1280.0f * Canvas->GetDPIScale()), Canvas->SizeY / (720.0f * Canvas->GetDPIScale()))};

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{180.0f * Scale};

	auto MaxVerticalPosition{VerticalPosition};
	auto HorizontalPosition{5.0f * Scale};

	static const auto DebugModeHeaderText{FText::AsCultureInvariant(TEXT("Debug mode is enabled! Press (Shift + 0) to disable."))};

	DisplayDebugHeader(Canvas, DebugModeHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	if (!DebugDisplay.IsDisplayOn(UAlsUtility::CurvesDisplayName()) && !DebugDisplay.IsDisplayOn(UAlsUtility::StateDisplayName()) &&
	    !DebugDisplay.IsDisplayOn(UAlsUtility::ShapesDisplayName()) && !DebugDisplay.IsDisplayOn(UAlsUtility::TracesDisplayName()) &&
	    !DebugDisplay.IsDisplayOn(UAlsUtility::MantlingDisplayName()))
	{
		VerticalPosition = MaxVerticalPosition;

		Super::DisplayDebug(Canvas, DebugDisplay, Unused, VerticalPosition);
		return;
	}

	const auto InitialVerticalPosition{VerticalPosition};

	static const auto CurvesHeaderText{FText::AsCultureInvariant(TEXT("ALS.Curves (Shift + 1)"))};

	if (DebugDisplay.IsDisplayOn(UAlsUtility::CurvesDisplayName()))
	{
		DisplayDebugHeader(Canvas, CurvesHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugCurves(Canvas, Scale, HorizontalPosition, VerticalPosition);

		MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition + RowOffset);
		VerticalPosition = InitialVerticalPosition;
		HorizontalPosition += ColumnOffset;
	}
	else
	{
		DisplayDebugHeader(Canvas, CurvesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);

		VerticalPosition += RowOffset;
	}

	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	static const auto StateHeaderText{FText::AsCultureInvariant(TEXT("ALS.State (Shift + 2)"))};

	if (DebugDisplay.IsDisplayOn(UAlsUtility::StateDisplayName()))
	{
		DisplayDebugHeader(Canvas, StateHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugState(Canvas, Scale, HorizontalPosition, VerticalPosition);
	}
	else
	{
		DisplayDebugHeader(Canvas, StateHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);
	}

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	static const auto ShapesHeaderText{FText::AsCultureInvariant(TEXT("ALS.Shapes (Shift + 3)"))};

	if (DebugDisplay.IsDisplayOn(UAlsUtility::ShapesDisplayName()))
	{
		DisplayDebugHeader(Canvas, ShapesHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugShapes(Canvas, Scale, HorizontalPosition, VerticalPosition);
	}
	else
	{
		DisplayDebugHeader(Canvas, ShapesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);
	}

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	static const auto TracesHeaderText{FText::AsCultureInvariant(TEXT("ALS.Traces (Shift + 4)"))};

	if (DebugDisplay.IsDisplayOn(UAlsUtility::TracesDisplayName()))
	{
		DisplayDebugHeader(Canvas, TracesHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugTraces(Canvas, Scale, HorizontalPosition, VerticalPosition);
	}
	else
	{
		DisplayDebugHeader(Canvas, TracesHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);
	}

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	static const auto MantlingHeaderText{FText::AsCultureInvariant(TEXT("ALS.Mantling (Shift + 5)"))};

	if (DebugDisplay.IsDisplayOn(UAlsUtility::MantlingDisplayName()))
	{
		DisplayDebugHeader(Canvas, MantlingHeaderText, FLinearColor::Green, Scale, HorizontalPosition, VerticalPosition);
		DisplayDebugMantling(Canvas, Scale, HorizontalPosition, VerticalPosition);
	}
	else
	{
		DisplayDebugHeader(Canvas, MantlingHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalPosition, VerticalPosition);
	}

	VerticalPosition += RowOffset;
	MaxVerticalPosition = FMath::Max(MaxVerticalPosition, VerticalPosition);

	VerticalPosition = MaxVerticalPosition;

	Super::DisplayDebug(Canvas, DebugDisplay, Unused, VerticalPosition);
}

void AAlsCharacter::DisplayDebugHeader(UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
                                       const float Scale, const float HorizontalPosition, float& VerticalPosition)
{
	FCanvasTextItem Text{
		{HorizontalPosition, VerticalPosition},
		HeaderText,
		GEngine->GetMediumFont(),
		HeaderColor
	};

	Text.Scale = {Scale, Scale};
	Text.EnableShadow(FLinearColor::Black);

	Text.Draw(Canvas->Canvas);

	VerticalPosition += 15.0f * Scale;
}

void AAlsCharacter::DisplayDebugCurves(UCanvas* Canvas, const float Scale, const float HorizontalPosition, float& VerticalPosition) const
{
	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{145.0f * Scale};

	TArray<FName> CurveNames;
	GetMesh()->GetAnimInstance()->GetAllCurveNames(CurveNames);

	CurveNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	for (const auto& CurveName : CurveNames)
	{
		const auto CurveValue{GetMesh()->GetAnimInstance()->GetCurveValue(CurveName)};

		Text.SetColor(FMath::Lerp(FLinearColor::Gray, FLinearColor::White, UAlsMath::Clamp01(FMath::Abs(CurveValue))));

		Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(CurveName.ToString(), false));
		Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

		Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), CurveValue));
		Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

		VerticalPosition += RowOffset;
	}
}

void AAlsCharacter::DisplayDebugState(UCanvas* Canvas, const float Scale, const float HorizontalPosition, float& VerticalPosition) const
{
	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{120.0f * Scale};

	static const auto DesiredStanceText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, DesiredStance), false))
	};

	Text.Text = DesiredStanceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(DesiredStance), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto StanceText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, Stance), false))
	};

	Text.Text = StanceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(Stance), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto DesiredGaitText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, DesiredGait), false))
	};

	Text.Text = DesiredGaitText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(DesiredGait), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto GaitText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, Gait), false))
	};

	Text.Text = GaitText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(Gait), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto DesiredAimingText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, bDesiredAiming), true))
	};

	Text.Text = DesiredAimingText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(ToString(bDesiredAiming), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto DesiredRotationModeText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, DesiredRotationMode), false))
	};

	Text.Text = DesiredRotationModeText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(DesiredRotationMode), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto RotationModeText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, RotationMode), false))
	};

	Text.Text = RotationModeText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(RotationMode), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto ViewModeText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, ViewMode), false))
	};

	Text.Text = ViewModeText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(ViewMode), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto OverlayModeText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, OverlayMode), false))
	};

	Text.Text = OverlayModeText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(OverlayMode), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto LocomotionModeText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, LocomotionMode), false))
	};

	Text.Text = LocomotionModeText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(LocomotionMode), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto LocomotionActionText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, LocomotionAction), false))
	};

	Text.Text = LocomotionActionText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(GetEnumValueString(LocomotionAction), false));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;
}

void AAlsCharacter::DisplayDebugShapes(UCanvas* Canvas, const float Scale, const float HorizontalPosition, float& VerticalPosition) const
{
	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{120.0f * Scale};

	static const auto AimingRotationText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, AimingRotation), false))
	};

	auto Color{FLinearColor::Red};
	Text.SetColor(Color);

	Text.Text = AimingRotationText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("R: %.2f P: %.2f Y: %.2f"),
	                                                      AimingRotation.Roll, AimingRotation.Pitch, AimingRotation.Yaw));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

#if ENABLE_DRAW_DEBUG
	DrawDebugCone(GetWorld(), LocomotionState.SmoothLocation + FVector{0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()},
	              AimingRotation.Vector(), 100.0f, FMath::DegreesToRadians(15.0f), FMath::DegreesToRadians(15.0f),
	              8, Color.ToFColor(true), false, -1.0f, SDPG_World, 1.0f);
#endif

	VerticalPosition += RowOffset;

	static const auto InputYawAngleText{
		FText::AsCultureInvariant(
			FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(FAlsLocomotionCharacterState, InputYawAngle), false))
	};

	Color = LocomotionState.bHasInput ? FLinearColor{1.0f, 0.5f, 0.0f} : FLinearColor{0.5f, 0.25f, 0.0f};
	Text.SetColor(Color);

	Text.Text = InputYawAngleText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), LocomotionState.InputYawAngle));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

#if ENABLE_DRAW_DEBUG
	const auto FeetLocation{LocomotionState.SmoothLocation - FVector(0.0f, 0.0f, GetCapsuleComponent()->Bounds.BoxExtent.Z)};

	DrawDebugDirectionalArrow(GetWorld(),
	                          FeetLocation + FVector{0.0f, 0.0f, 3.0f},
	                          FeetLocation + FVector{0.0f, 0.0f, 3.0f} +
	                          FVector{UAlsMath::AngleToDirection(LocomotionState.InputYawAngle) * 50.0f, 0.0f},
	                          50.0f, Color.ToFColor(true), false, -1.0f, SDPG_World, 3.0f);
#endif

	VerticalPosition += RowOffset;

	static const auto SpeedText{
		FText::AsCultureInvariant(FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(FAlsLocomotionCharacterState, Speed), false))
	};

	Color = LocomotionState.bHasSpeed ? FLinearColor{0.75f, 0.0f, 1.0f} : FLinearColor{0.375f, 0.0f, 0.5f};
	Text.SetColor(Color);

	Text.Text = SpeedText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), LocomotionState.Speed));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto VelocityDirectionText{FText::AsCultureInvariant(TEXT("Velocity Direction"))};

	const auto VelocityDirection{LocomotionState.Velocity.GetSafeNormal()};

	Text.Text = VelocityDirectionText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"),
	                                                      VelocityDirection.X, VelocityDirection.Y, VelocityDirection.Z));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto VelocityYawAngleText{
		FText::AsCultureInvariant(
			FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(FAlsLocomotionCharacterState, VelocityYawAngle), false))
	};

	Text.Text = VelocityYawAngleText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%.2f"), LocomotionState.VelocityYawAngle));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

#if ENABLE_DRAW_DEBUG
	DrawDebugDirectionalArrow(GetWorld(),
	                          FeetLocation,
	                          FeetLocation +
	                          FVector{UAlsMath::AngleToDirection(LocomotionState.VelocityYawAngle), 0.0f} *
	                          FMath::GetMappedRangeValueClamped({0.0f, AlsCharacterMovement->GetMaxSpeed()},
	                                                            {50.0f, 75.0f}, LocomotionState.Speed),
	                          50.0f, Color.ToFColor(true), false, -1.0f, SDPG_World, 3.0f);
#endif

	VerticalPosition += RowOffset;

	static const auto TargetActorRotationText{
		FText::AsCultureInvariant(
			FName::NameToDisplayString(GET_MEMBER_NAME_STRING_CHECKED(FAlsLocomotionCharacterState, TargetActorRotation), false))
	};

	Color = {0.0f, 0.75f, 1.0f};
	Text.SetColor(Color);

	Text.Text = TargetActorRotationText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("R: %.2f P: %.2f Y: %.2f"),
	                                                      LocomotionState.TargetActorRotation.Roll,
	                                                      LocomotionState.TargetActorRotation.Pitch,
	                                                      LocomotionState.TargetActorRotation.Yaw));
	Text.Draw(Canvas->Canvas, {HorizontalPosition + ColumnOffset, VerticalPosition});

#if ENABLE_DRAW_DEBUG
	DrawDebugDirectionalArrow(GetWorld(),
	                          FeetLocation + FVector{0.0f, 0.0f, 6.0f},
	                          FeetLocation + FVector{0.0f, 0.0f, 6.0f} + LocomotionState.TargetActorRotation.Vector() * 50.0f,
	                          50.0f, Color.ToFColor(true), false, -1.0f, SDPG_World, 3.0f);
#endif

	VerticalPosition += RowOffset;

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(GetWorld(), LocomotionState.SmoothLocation, GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
	                 GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetComponentQuat(),
	                 FColor::Green, false, -1.0f, SDPG_World, 1.0f);
#endif
}

void AAlsCharacter::DisplayDebugTraces(UCanvas* Canvas, const float Scale, const float HorizontalPosition, float& VerticalPosition) const
{
	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};

	static const auto FootOffsetTraceText{FText::AsCultureInvariant(TEXT("Foot Offset"))};

	Text.SetColor({0.0f, 0.75f, 1.0f});

	Text.Text = FootOffsetTraceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto GroundPredictionTraceText{FText::AsCultureInvariant(TEXT("Ground Prediction"))};

	Text.SetColor({0.75f, 0.0f, 1.0f});

	Text.Text = GroundPredictionTraceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto FootstepEffectsTraceText{FText::AsCultureInvariant(TEXT("Footstep Effects"))};

	Text.SetColor(FLinearColor::Red);

	Text.Text = FootstepEffectsTraceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;
}

void AAlsCharacter::DisplayDebugMantling(UCanvas* Canvas, const float Scale, const float HorizontalPosition, float& VerticalPosition) const
{
	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};

	static const auto ForwardTraceText{FText::AsCultureInvariant(TEXT("Forward Trace"))};

	Text.SetColor({0.0f, 0.75f, 1.0f});

	Text.Text = ForwardTraceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto DownwardTraceText{FText::AsCultureInvariant(TEXT("Downward Trace"))};

	Text.SetColor({0.75f, 0.0f, 1.0f});

	Text.Text = DownwardTraceText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;

	static const auto FreeSpaceOverlapText{FText::AsCultureInvariant(TEXT("Free Space Overlap (Only if Failed)"))};

	Text.SetColor(FLinearColor::Red);

	Text.Text = FreeSpaceOverlapText;
	Text.Draw(Canvas->Canvas, {HorizontalPosition, VerticalPosition});

	VerticalPosition += RowOffset;
}
