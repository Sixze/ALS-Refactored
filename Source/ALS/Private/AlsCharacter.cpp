#include "AlsCharacter.h"

#include "AlsAnimationInstance.h"
#include "AlsCharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/GameNetworkManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Settings/AlsCharacterSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsLog.h"
#include "Utility/AlsMath.h"
#include "Utility/GameplayTags/AlsLocomotionActionTags.h"
#include "Utility/GameplayTags/AlsLocomotionModeTags.h"

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

	// This will prevent the editor from combining component details with actor details.
	// Component details can still be accessed from the actor's component hierarchy.

#if WITH_EDITOR
	StaticClass()->FindPropertyByName(TEXT("Mesh"))->SetPropertyFlags(CPF_DisableEditOnInstance);
	StaticClass()->FindPropertyByName(TEXT("CapsuleComponent"))->SetPropertyFlags(CPF_DisableEditOnInstance);
	StaticClass()->FindPropertyByName(TEXT("CharacterMovement"))->SetPropertyFlags(CPF_DisableEditOnInstance);
#endif
}

#if WITH_EDITOR
bool AAlsCharacter::CanEditChange(const FProperty* Property) const
{
	auto bCanEditChange{Super::CanEditChange(Property)};

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, bUseControllerRotationPitch) ||
	    Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, bUseControllerRotationYaw) ||
	    Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, bUseControllerRotationRoll))
	{
		bCanEditChange = false;
	}

	return bCanEditChange;
}
#endif

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

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ViewRotation, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InputDirection, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RagdollTargetLocation, Parameters)
}

void AAlsCharacter::PostInitializeComponents()
{
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

	AlsAnimationInstance = Cast<UAlsAnimationInstance>(GetMesh()->GetAnimInstance());

	// Set default rotation values.

	ViewState.Rotation = ViewRotation;
	ViewState.PreviousYawAngle = ViewRotation.Yaw;

	RefreshLocomotionLocationAndRotation();
	RefreshTargetYawAngleUsingLocomotionRotation();

	LocomotionState.InputYawAngle = LocomotionState.Rotation.Yaw;
	LocomotionState.VelocityYawAngle = LocomotionState.Rotation.Yaw;
}

void AAlsCharacter::BeginPlay()
{
	check(IsValid(Settings))
	check(IsValid(MovementSettings))
	check(IsValid(AlsAnimationInstance))

	checkf(!bUseControllerRotationPitch && !bUseControllerRotationYaw && !bUseControllerRotationRoll,
	       TEXT("These settings are not allowed and must be turned off!"))

	Super::BeginPlay();

	// Update states to use the initial desired values.

	RefreshRotationMode();

	AlsCharacterMovement->SetRotationMode(RotationMode);

	ApplyDesiredStance();

	AlsCharacterMovement->SetStance(Stance);

	RefreshGait();

	OnOverlayModeChanged(OverlayMode);
}

void AAlsCharacter::PostNetReceiveLocationAndRotation()
{
	if (GetLocalRole() <= ROLE_SimulatedProxy)
	{
		// Ignore server-replicated rotation on simulated proxies because ALS itself has full control over character rotation.

		GetReplicatedMovement_Mutable().Rotation = GetActorRotation();
	}

	Super::PostNetReceiveLocationAndRotation();
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

	RefreshLocomotionLocationAndRotation();

	RefreshRotationMode();

	RefreshView(DeltaTime);

	RefreshLocomotion(DeltaTime);
	RefreshGait();

	RefreshGroundedActorRotation(DeltaTime);
	RefreshInAirActorRotation(DeltaTime);

	TryStartMantlingInAir();

	RefreshMantling();
	RefreshRagdolling(DeltaTime);

	if (LocomotionState.bRotationLocked)
	{
		RefreshViewRelativeTargetYawAngle();
	}
	else if (!LocomotionMode.IsValid() || LocomotionAction.IsValid())
	{
		RefreshLocomotionLocationAndRotation();
		RefreshTargetYawAngleUsingLocomotionRotation();
	}

	LocomotionState.PreviousVelocity = LocomotionState.Velocity;
	ViewState.PreviousYawAngle = ViewState.Rotation.Yaw;

	Super::Tick(DeltaTime);

	if (!GetMesh()->bRecentlyRendered &&
	    GetMesh()->VisibilityBasedAnimTickOption > EVisibilityBasedAnimTickOption::AlwaysTickPose)
	{
		AlsAnimationInstance->SetPendingUpdate(true);
	}

	AlsAnimationInstance->SetAnimationCurvesRelevant(
		GetMesh()->bRecentlyRendered ||
		GetMesh()->VisibilityBasedAnimTickOption <= EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones);
}

void AAlsCharacter::Restart()
{
	Super::Restart();

	ApplyDesiredStance();
}

void AAlsCharacter::FaceRotation(const FRotator NewRotation, const float DeltaTime)
{
	// Left empty intentionally.
}

void AAlsCharacter::Jump()
{
	if (Stance == EAlsStance::Standing && !LocomotionAction.IsValid() &&
	    LocomotionMode == FAlsLocomotionModeTags::Get().Grounded)
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
			SetLocomotionMode(FAlsLocomotionModeTags::Get().Grounded);
			break;

		case MOVE_Falling:
			SetLocomotionMode(FAlsLocomotionModeTags::Get().InAir);
			break;

		default:
			SetLocomotionMode(FGameplayTag::EmptyTag);
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
		if (LocomotionMode == FAlsLocomotionModeTags::Get().Grounded)
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
		else if (LocomotionMode == FAlsLocomotionModeTags::Get().InAir)
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
	if (LocomotionMode != FAlsLocomotionModeTags::Get().Grounded)
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

	static constexpr auto ViewRelativeAngleThreshold{50.0f};

	return LocomotionState.bHasInput &&
	       (RotationMode != EAlsRotationMode::Aiming || Settings->bSprintHasPriorityOverAiming) &&
	       // ReSharper disable once CppRedundantParentheses
	       ((ViewMode != EAlsViewMode::FirstPerson &&
	         (DesiredRotationMode == EAlsRotationMode::VelocityDirection || Settings->bRotateToVelocityWhenSprinting)) ||
	        FMath::Abs(FRotator::NormalizeAxis(LocomotionState.InputYawAngle - ViewRotation.Yaw)) < ViewRelativeAngleThreshold);
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
			SetRotationMode(!Settings->bAllowAimingWhenInAir && LocomotionMode == FAlsLocomotionModeTags::Get().InAir
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
		SetRotationMode(!Settings->bAllowAimingWhenInAir && LocomotionMode == FAlsLocomotionModeTags::Get().InAir
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

void AAlsCharacter::SetLocomotionMode(const FGameplayTag& NewModeTag)
{
	if (LocomotionMode != NewModeTag)
	{
		const auto PreviousMode{LocomotionMode};

		LocomotionMode = NewModeTag;

		NotifyLocomotionModeChanged(PreviousMode);
	}
}

void AAlsCharacter::NotifyLocomotionModeChanged(const FGameplayTag& PreviousModeTag)
{
	ApplyDesiredStance();

	if (LocomotionMode == FAlsLocomotionModeTags::Get().Grounded &&
	    PreviousModeTag == FAlsLocomotionModeTags::Get().InAir)
	{
		if (Settings->Ragdolling.bStartRagdollingOnLand &&
		    LocomotionState.Velocity.Z <= -Settings->Ragdolling.RagdollingOnLandSpeedThreshold)
		{
			StartRagdolling();
		}
		else if (Settings->Rolling.bStartRollingOnLand &&
		         LocomotionState.Velocity.Z <= -Settings->Rolling.RollingOnLandSpeedThreshold)
		{
			static constexpr auto PlayRate{1.3f};

			StartRolling(PlayRate, LocomotionState.bHasSpeed
				                       ? LocomotionState.VelocityYawAngle
				                       : GetActorRotation().Yaw);
		}
		else
		{
			static constexpr auto HasInputBrakingFrictionFactor{0.5f};
			static constexpr auto NoInputBrakingFrictionFactor{3.0f};

			GetCharacterMovement()->BrakingFrictionFactor = LocomotionState.bHasInput
				                                                ? HasInputBrakingFrictionFactor
				                                                : NoInputBrakingFrictionFactor;

			static constexpr auto ResetDelay{0.5f};

			GetWorldTimerManager().SetTimer(BrakingFrictionFactorResetTimer, FTimerDelegate::CreateWeakLambda(
				                                this, [&BrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor]
				                                {
					                                BrakingFrictionFactor = 0.0f;
				                                }), ResetDelay, false);
		}
	}
	else if (LocomotionMode == FAlsLocomotionModeTags::Get().InAir &&
	         LocomotionAction == FAlsLocomotionActionTags::Get().Rolling &&
	         Settings->Rolling.bInterruptRollingWhenInAir)
	{
		// If the character is currently rolling, then enable ragdolling.

		StartRagdolling();
	}

	OnLocomotionModeChanged(PreviousModeTag);
}

void AAlsCharacter::OnLocomotionModeChanged_Implementation(const FGameplayTag& PreviousModeTag) {}

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

		// The character movement component already sends the view rotation to the
		// server if the movement is replicated, so we don't have to do it ourselves.

		if (!IsReplicatingMovement() && GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetViewRotation(NewViewRotation);
		}
	}
}

void AAlsCharacter::OnReplicate_ViewRotation(const FRotator& PreviousViewRotation)
{
	ViewRotation.Normalize();

	CorrectViewInterpolation(PreviousViewRotation);
}

void AAlsCharacter::CorrectViewInterpolation(const FRotator& PreviousViewRotation)
{
	// Based on UCharacterMovementComponent::SmoothCorrection().

	if (!Settings->bViewInterpolationEnabled || GetLocalRole() >= ROLE_AutonomousProxy ||
	    GetReplicatedServerLastTransformUpdateTimeStamp() <= 0.0f)
	{
		return;
	}

	const auto NewViewRotation{ViewRotation};

	ViewState.InterpolationInitialRotation = PreviousViewRotation;
	ViewState.InterpolationTargetRotation = NewViewRotation;
	ViewRotation = PreviousViewRotation;

	// Using server time lets us know how much time actually elapsed, regardless of packet lag variance.

	const auto ServerDeltaTime{GetReplicatedServerLastTransformUpdateTimeStamp() - ViewState.InterpolationServerTimeStamp};

	// Don't let the client fall too far behind or run ahead of new server time.

	const auto MaxServerDeltaTime{GetDefault<AGameNetworkManager>()->MaxClientSmoothingDeltaTime};
	const auto MinServerDeltaTime{FMath::Min(GetCharacterMovement()->NetworkSimulatedSmoothLocationTime, MaxServerDeltaTime)};

	// Calculate how far behind we can be after receiving a new server time.

	const auto MinClientDeltaTime{FMath::Clamp(ServerDeltaTime * 1.25f, MinServerDeltaTime, MaxServerDeltaTime)};

	ViewState.InterpolationServerTimeStamp = GetReplicatedServerLastTransformUpdateTimeStamp();

	ViewState.InterpolationClientTimeStamp = FMath::Clamp(ViewState.InterpolationClientTimeStamp,
	                                                      ViewState.InterpolationServerTimeStamp - MinClientDeltaTime,
	                                                      ViewState.InterpolationServerTimeStamp);

	// Compute actual delta between new server time and client simulation.

	ViewState.InterpolationDuration = ViewState.InterpolationServerTimeStamp - ViewState.InterpolationClientTimeStamp;
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

	RefreshViewInterpolation(DeltaTime);

	// Interpolate view rotation to current raw view rotation for smooth character
	// rotation movement. Decrease interpolation speed for slower but smoother movement.

	static constexpr auto InterpolationSpeed{30.0f};

	ViewState.Rotation = UAlsMath::ExponentialDecay(ViewState.Rotation, ViewRotation, DeltaTime, InterpolationSpeed);

	// Set the yaw speed by comparing the current and previous view yaw angle, divided
	// by delta seconds. This represents the speed the camera is rotating left to right.

	ViewState.YawSpeed = FMath::Abs((ViewState.Rotation.Yaw - ViewState.PreviousYawAngle) / DeltaTime);
}

void AAlsCharacter::RefreshViewInterpolation(const float DeltaTime)
{
	// Based on UCharacterMovementComponent::SmoothClientPosition_Interpolate()
	// and UCharacterMovementComponent::SmoothClientPosition_UpdateVisuals().

	if (!Settings->bViewInterpolationEnabled || GetLocalRole() >= ROLE_AutonomousProxy)
	{
		return;
	}

	ViewState.InterpolationClientTimeStamp += DeltaTime;

	const auto InterpolationAmount{
		ViewState.InterpolationDuration <= SMALL_NUMBER || ViewState.InterpolationClientTimeStamp >= ViewState.InterpolationServerTimeStamp
			? 1.0f
			: FMath::Max(0.0f,
			             1.0f - (ViewState.InterpolationServerTimeStamp - ViewState.InterpolationClientTimeStamp) /
			             ViewState.InterpolationDuration)
	};

	if (!FAnimWeight::IsFullWeight(InterpolationAmount))
	{
		ViewRotation = UAlsMath::LerpRotator(ViewState.InterpolationInitialRotation,
		                                     ViewState.InterpolationTargetRotation, InterpolationAmount);
	}
	else
	{
		ViewRotation = ViewState.InterpolationTargetRotation;
		ViewState.InterpolationClientTimeStamp = ViewState.InterpolationServerTimeStamp;
	}
}

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
		LocomotionState.RotationQuaternion = ActorTransform.GetRotation();
		LocomotionState.Rotation = LocomotionState.RotationQuaternion.Rotator();
		return;
	}

	const auto SmoothTransform{
		ActorTransform * FTransform{
			GetMesh()->GetRelativeRotationCache().RotatorToQuat(GetMesh()->GetRelativeRotation()) * GetBaseRotationOffset().Inverse(),
			GetMesh()->GetRelativeLocation() - GetBaseTranslationOffset()
		}
	};

	LocomotionState.Location = SmoothTransform.GetLocation();
	LocomotionState.RotationQuaternion = SmoothTransform.GetRotation();
	LocomotionState.Rotation = LocomotionState.RotationQuaternion.Rotator();
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
	LocomotionState.bHasSpeed = LocomotionState.Speed >= 1.0f;

	if (LocomotionState.bHasSpeed)
	{
		LocomotionState.VelocityYawAngle = UAlsMath::DirectionToAngle2D(LocomotionState.Velocity);
	}

	// Character is moving if has speed and current acceleration, or if the speed is greater than moving speed threshold.

	LocomotionState.bMoving = LocomotionState.bHasInput || LocomotionState.Speed > Settings->MovingSpeedThreshold;

	LocomotionState.Acceleration = (LocomotionState.Velocity - LocomotionState.PreviousVelocity) / DeltaTime;
}

void AAlsCharacter::RefreshGroundedActorRotation(const float DeltaTime)
{
	if (LocomotionState.bRotationLocked || LocomotionAction.IsValid() ||
	    LocomotionMode != FAlsLocomotionModeTags::Get().Grounded)
	{
		return;
	}

	if (HasAnyRootMotion())
	{
		RefreshTargetYawAngleUsingLocomotionRotation();
		return;
	}

	if (!LocomotionState.bMoving)
	{
		// Not moving.

		ApplyRotationYawSpeedFromCharacter(DeltaTime);

		if (TryRefreshCustomGroundedNotMovingActorRotation(DeltaTime))
		{
			return;
		}

		if (RotationMode == EAlsRotationMode::Aiming || ViewMode == EAlsViewMode::FirstPerson)
		{
			RefreshGroundedNotMovingAimingActorRotation(DeltaTime);
			return;
		}

		RefreshTargetYawAngleUsingLocomotionRotation();
		return;
	}

	// Moving.

	if (TryRefreshCustomGroundedMovingActorRotation(DeltaTime))
	{
		return;
	}

	switch (RotationMode)
	{
		case EAlsRotationMode::VelocityDirection:
		{
			static constexpr auto TargetYawAngleRotationSpeed{800.0f};

			RefreshActorRotationExtraSmooth(LocomotionState.VelocityYawAngle, DeltaTime, CalculateActorRotationInterpolationSpeed(),
			                                TargetYawAngleRotationSpeed);
		}
		break;

		case EAlsRotationMode::LookingDirection:
		{
			const auto TargetYawAngle{
				Gait == EAlsGait::Sprinting
					? LocomotionState.VelocityYawAngle
					: FRotator::NormalizeAxis(ViewState.Rotation.Yaw +
					                          GetMesh()->GetAnimInstance()->GetCurveValue(UAlsConstants::RotationYawOffsetCurve()))
			};

			static constexpr auto TargetYawAngleRotationSpeed{500.0f};

			RefreshActorRotationExtraSmooth(TargetYawAngle, DeltaTime, CalculateActorRotationInterpolationSpeed(),
			                                TargetYawAngleRotationSpeed);
		}
		break;

		case EAlsRotationMode::Aiming:
			RefreshGroundedMovingAimingActorRotation(DeltaTime);
			break;

		default:
			RefreshTargetYawAngleUsingLocomotionRotation();
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
	static constexpr auto ActorRotationInterpolationSpeed{20.0f};
	static constexpr auto TargetYawAngleRotationSpeed{1000.0f};

	RefreshActorRotationExtraSmooth(ViewState.Rotation.Yaw, DeltaTime, ActorRotationInterpolationSpeed, TargetYawAngleRotationSpeed);
}

void AAlsCharacter::RefreshGroundedNotMovingAimingActorRotation(const float DeltaTime)
{
	if (LocomotionState.bHasInput)
	{
		static constexpr auto ActorRotationInterpolationSpeed{20.0f};
		static constexpr auto TargetYawAngleRotationSpeed{1000.0f};

		RefreshActorRotationExtraSmooth(ViewState.Rotation.Yaw, DeltaTime, ActorRotationInterpolationSpeed, TargetYawAngleRotationSpeed);
		return;
	}

	// Prevent the character from rotating past a certain angle.

	static constexpr auto ViewRelativeActorYawAngleThreshold{70.0f};

	const auto ViewRelativeActorYawAngle{FRotator::NormalizeAxis(ViewState.Rotation.Yaw - LocomotionState.Rotation.Yaw)};
	if (FMath::Abs(ViewRelativeActorYawAngle) > ViewRelativeActorYawAngleThreshold)
	{
		static constexpr auto ActorRotationInterpolationSpeed{20.0f};
		RefreshActorRotation(FRotator::NormalizeAxis(ViewState.Rotation.Yaw + (ViewRelativeActorYawAngle >= 0.0f
			                                                                       ? -ViewRelativeActorYawAngleThreshold
			                                                                       : ViewRelativeActorYawAngleThreshold)),
		                     DeltaTime, ActorRotationInterpolationSpeed);
	}
	else
	{
		RefreshTargetYawAngleUsingLocomotionRotation();
	}
}

float AAlsCharacter::CalculateActorRotationInterpolationSpeed() const
{
	// Calculate the rotation speed by using the rotation speed curve in the movement gait settings. Using
	// the curve in conjunction with the gait amount gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	static constexpr auto ReferenceViewYawSpeed{300.0f};
	static constexpr auto InterpolationSpeedMultiplier{3.0f};

	return AlsCharacterMovement->GetGaitSettings().RotationInterpolationSpeedCurve->
	                             GetFloatValue(AlsCharacterMovement->CalculateGaitAmount()) *
	       UAlsMath::LerpClamped(1.0f, InterpolationSpeedMultiplier, ViewState.YawSpeed / ReferenceViewYawSpeed);
}

void AAlsCharacter::ApplyRotationYawSpeedFromAnimationInstance(const float DeltaTime)
{
	if (GetLocalRole() >= ROLE_Authority && !IsLocallyControlled() &&
	    !LocomotionState.bRotationLocked && !LocomotionAction.IsValid() &&
	    LocomotionMode == FAlsLocomotionModeTags::Get().Grounded &&
	    !HasAnyRootMotion() && !LocomotionState.bMoving)
	{
		ApplyRotationYawSpeed(DeltaTime);
	}
}

void AAlsCharacter::ApplyRotationYawSpeedFromCharacter(const float DeltaTime)
{
	if (GetLocalRole() >= ROLE_Authority && !IsLocallyControlled())
	{
		return;
	}

	ApplyRotationYawSpeed(DeltaTime);
}

void AAlsCharacter::ApplyRotationYawSpeed(const float DeltaTime)
{
	const auto DeltaYawAngle{GetMesh()->GetAnimInstance()->GetCurveValue(UAlsConstants::RotationYawSpeedCurve()) * DeltaTime};
	if (FMath::Abs(DeltaYawAngle) > SMALL_NUMBER)
	{
		AddActorWorldRotation({0.0f, DeltaYawAngle, 0.0f});

		RefreshLocomotionLocationAndRotation();
		RefreshTargetYawAngleUsingLocomotionRotation();
	}
}

void AAlsCharacter::RefreshInAirActorRotation(const float DeltaTime)
{
	if (LocomotionState.bRotationLocked || LocomotionAction.IsValid() ||
	    LocomotionMode != FAlsLocomotionModeTags::Get().InAir)
	{
		return;
	}

	if (TryRefreshCustomInAirActorRotation(DeltaTime))
	{
		return;
	}

	static constexpr auto ActorRotationInterpolationSpeed{5.0f};

	switch (RotationMode)
	{
		case EAlsRotationMode::VelocityDirection:
		case EAlsRotationMode::LookingDirection:
			switch (Settings->InAirRotationMode)
			{
				case EAlsInAirRotationMode::RotateToVelocityOnJump:
					if (LocomotionState.bMoving)
					{
						RefreshActorRotation(LocomotionState.VelocityYawAngle, DeltaTime, ActorRotationInterpolationSpeed);
					}
					else
					{
						RefreshTargetYawAngleUsingLocomotionRotation();
					}
					break;

				case EAlsInAirRotationMode::KeepRelativeRotation:
					RefreshActorRotation(ViewState.Rotation.Yaw - LocomotionState.ViewRelativeTargetYawAngle,
					                     DeltaTime, ActorRotationInterpolationSpeed);
					break;

				default:
					RefreshTargetYawAngleUsingLocomotionRotation();
					break;
			}
			break;

		case EAlsRotationMode::Aiming:
			RefreshInAirAimingActorRotation(DeltaTime);
			break;

		default:
			RefreshTargetYawAngleUsingLocomotionRotation();
			break;
	}
}

bool AAlsCharacter::TryRefreshCustomInAirActorRotation(const float DeltaTime)
{
	return false;
}

void AAlsCharacter::RefreshInAirAimingActorRotation(const float DeltaTime)
{
	static constexpr auto ActorRotationInterpolationSpeed{15.0f};

	RefreshActorRotation(ViewState.Rotation.Yaw, DeltaTime, ActorRotationInterpolationSpeed);
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
	LocomotionState.TargetYawAngle = TargetYawAngle;

	RefreshViewRelativeTargetYawAngle();

	// Interpolate target yaw angle for extra smooth rotation.

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
