#include "AlsCharacterMovementComponent.h"

#include "AlsCharacter.h"
#include "Curves/CurveVector.h"

void FAlsSavedMove::Clear()
{
	Super::Clear();

	Stance = EAlsStance::Standing;
	RotationMode = EAlsRotationMode::LookingDirection;
	MaxAllowedGait = EAlsGait::Walking;
}

void FAlsSavedMove::SetMoveFor(ACharacter* Character, const float NewDeltaTime, const FVector& NewAcceleration,
                               FNetworkPredictionData_Client_Character& PredictionData)
{
	Super::SetMoveFor(Character, NewDeltaTime, NewAcceleration, PredictionData);

	const auto* Movement{Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement())};
	if (IsValid(Movement))
	{
		Stance = Movement->Stance;
		RotationMode = Movement->RotationMode;
		MaxAllowedGait = Movement->MaxAllowedGait;
	}
}

bool FAlsSavedMove::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, const float MaxDelta) const
{
	const auto* NewMove{static_cast<FAlsSavedMove*>(NewMovePtr.Get())};

	return Stance == NewMove->Stance &&
	       RotationMode == NewMove->RotationMode &&
	       MaxAllowedGait == NewMove->MaxAllowedGait &&
	       Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}

void FAlsSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	auto* Movement{Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement())};
	if (IsValid(Movement))
	{
		Movement->Stance = Stance;
		Movement->RotationMode = RotationMode;
		Movement->MaxAllowedGait = MaxAllowedGait;

		Movement->RefreshGaitSettings();
	}
}

FAlsNetworkPredictionData::FAlsNetworkPredictionData(const UCharacterMovementComponent& MovementComponent) : Super(MovementComponent) {}

FSavedMovePtr FAlsNetworkPredictionData::AllocateNewMove()
{
	return MakeShared<FAlsSavedMove>();
}

FNetworkPredictionData_Client* UAlsCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		auto* MutableThis{const_cast<UAlsCharacterMovementComponent*>(this)};

		MutableThis->ClientPredictionData = new FAlsNetworkPredictionData{*this};
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.0f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.0f;
	}

	return ClientPredictionData;
}

UAlsCharacterMovementComponent::UAlsCharacterMovementComponent()
{
	MaxWalkSpeed = 175.0f;
	MaxWalkSpeedCrouched = 150.0;
	MaxAcceleration = 1500.0f;

	BrakingFrictionFactor = 0.0f;
	CrouchedHalfHeight = 56.0f;
	bRunPhysicsWithNoController = true;

	MinAnalogWalkSpeed = 25.0f;
	bCanWalkOffLedgesWhenCrouching = true;
	PerchRadiusThreshold = 20.0f;
	PerchAdditionalHeight = 0.0f;
	LedgeCheckThreshold = 0.0f;

	AirControl = 0.15f;
	BrakingDecelerationFlying = 1000.0f;

	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanFly = true;
	bUseAccelerationForPaths = true;

	// https://unrealengine.hatenablog.com/entry/2019/01/16/231404

	FallingLateralFriction = 1.0f;
	JumpOffJumpZFactor = 0.0f;
}

float UAlsCharacterMovementComponent::GetMaxAcceleration() const
{
	// Get the acceleration using the movement curve. This allows for fine control over movement behavior at each speed.

	return IsMovingOnGround() && IsValid(GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve)
		       ? GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->GetVectorValue(CalculateGaitAmount()).X
		       : Super::GetMaxAcceleration();
}

float UAlsCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	// Get the deceleration using the movement curve. This allows for fine control over movement behavior at each speed.

	return IsMovingOnGround() && IsValid(GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve)
		       ? GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->GetVectorValue(CalculateGaitAmount()).Y
		       : Super::GetMaxBrakingDeceleration();
}

void UAlsCharacterMovementComponent::PhysWalking(const float DeltaTime, const int32 Iterations)
{
	if (IsValid(GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve))
	{
		// Get the ground friction using the movement curve. This allows for fine control over movement behavior at each speed.

		GroundFriction = GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->GetVectorValue(CalculateGaitAmount()).Z;
	}

	Super::PhysWalking(DeltaTime, Iterations);
}

void UAlsCharacterMovementComponent::SetMovementSettings(UAlsMovementCharacterSettings* NewMovementSettings)
{
	MovementSettings = NewMovementSettings;

	RefreshGaitSettings();
}

void UAlsCharacterMovementComponent::RefreshGaitSettings()
{
	GaitSettings = IsValid(MovementSettings)
		               ? *MovementSettings->GetMovementStanceSettingsForRotationMode(RotationMode)->GetMovementGaitSettingsForStance(Stance)
		               : FAlsMovementGaitSettings{};

	RefreshMaxWalkSpeed();
}

void UAlsCharacterMovementComponent::SetStance(const EAlsStance NewStance)
{
	if (Stance != NewStance)
	{
		Stance = NewStance;

		RefreshGaitSettings();

		if (PawnOwner->GetLocalRole() == ROLE_AutonomousProxy && PawnOwner->IsLocallyControlled())
		{
			ServerSetStance(Stance);
		}
	}
}

void UAlsCharacterMovementComponent::ServerSetStance_Implementation(const EAlsStance NewStance)
{
	Stance = NewStance;

	RefreshGaitSettings();
}

void UAlsCharacterMovementComponent::SetRotationMode(const EAlsRotationMode NewMode)
{
	if (RotationMode != NewMode)
	{
		RotationMode = NewMode;

		RefreshGaitSettings();

		if (PawnOwner->GetLocalRole() == ROLE_AutonomousProxy && PawnOwner->IsLocallyControlled())
		{
			ServerSetRotationMode(RotationMode);
		}
	}
}

void UAlsCharacterMovementComponent::ServerSetRotationMode_Implementation(const EAlsRotationMode NewMode)
{
	RotationMode = NewMode;

	RefreshGaitSettings();
}

void UAlsCharacterMovementComponent::SetMaxAllowedGait(const EAlsGait NewGait)
{
	if (MaxAllowedGait != NewGait)
	{
		MaxAllowedGait = NewGait;

		RefreshMaxWalkSpeed();

		if (PawnOwner->GetLocalRole() == ROLE_AutonomousProxy && PawnOwner->IsLocallyControlled())
		{
			ServerSetMaxAllowedGait(NewGait);
		}
	}
}

void UAlsCharacterMovementComponent::ServerSetMaxAllowedGait_Implementation(const EAlsGait NewGait)
{
	MaxAllowedGait = NewGait;

	RefreshMaxWalkSpeed();
}

void UAlsCharacterMovementComponent::RefreshMaxWalkSpeed()
{
	MaxWalkSpeed = GaitSettings.GetSpeedForGait(MaxAllowedGait);
	MaxWalkSpeedCrouched = MaxWalkSpeed;
}

float UAlsCharacterMovementComponent::CalculateGaitAmount() const
{
	// Map the character's current speed to the configured movement speeds ranging from 0 to 3,
	// where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting. This allows us to vary
	// the movement speeds but still use the mapped range in calculations for consistent results.

	const auto Speed{Velocity.Size2D()};

	if (Speed <= GaitSettings.WalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped({0.0f, GaitSettings.WalkSpeed}, {0.0f, 1.0f}, Speed);
	}

	if (Speed <= GaitSettings.RunSpeed)
	{
		return FMath::GetMappedRangeValueClamped({GaitSettings.WalkSpeed, GaitSettings.RunSpeed}, {1.0f, 2.0f}, Speed);
	}

	return FMath::GetMappedRangeValueClamped({GaitSettings.RunSpeed, GaitSettings.SprintSpeed}, {2.0f, 3.0f}, Speed);
}
