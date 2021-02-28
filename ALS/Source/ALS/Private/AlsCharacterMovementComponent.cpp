#include "AlsCharacterMovementComponent.h"

#include "AlsCharacter.h"
#include "Curves/CurveVector.h"

void FAlsSavedMove::Clear()
{
	Super::Clear();

	bMovementSettingsChangeRequested = false;
}

uint8 FAlsSavedMove::GetCompressedFlags() const
{
	auto Result = Super::GetCompressedFlags();

	if (bMovementSettingsChangeRequested)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

void FAlsSavedMove::SetMoveFor(ACharacter* Character, const float NewDeltaTime, FVector const& NewAcceleration,
                               FNetworkPredictionData_Client_Character& Data)
{
	Super::SetMoveFor(Character, NewDeltaTime, NewAcceleration, Data);

	const auto* CharacterMovement{Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement())};
	if (IsValid(CharacterMovement))
	{
		bMovementSettingsChangeRequested = CharacterMovement->IsMovementSettingsChangeRequested();
		MaxSpeed = CharacterMovement->GetCustomMaxWalkSpeed();
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

		MutableThis->ClientPredictionData = new FAlsNetworkPredictionData(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140;
	}

	return ClientPredictionData;
}

UAlsCharacterMovementComponent::UAlsCharacterMovementComponent()
{
	MaxAcceleration = 1500;
	BrakingFrictionFactor = 0;
	CrouchedHalfHeight = 56;
	bRunPhysicsWithNoController = true;

	MinAnalogWalkSpeed = 25;
	bCanWalkOffLedgesWhenCrouching = true;
	PerchRadiusThreshold = 20;
	PerchAdditionalHeight = 0;
	LedgeCheckThreshold = 0;

	AirControl = 0.15;
	BrakingDecelerationFlying = 1000;

	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanFly = true;
	bUseAccelerationForPaths = true;
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

void UAlsCharacterMovementComponent::OnMovementUpdated(const float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (bMovementSettingsChangeRequested && IsValid(CharacterOwner))
	{
		MaxWalkSpeed = CustomMaxWalkSpeed;
		MaxWalkSpeedCrouched = CustomMaxWalkSpeed;
	}
}

void UAlsCharacterMovementComponent::UpdateFromCompressedFlags(const uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bMovementSettingsChangeRequested = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UAlsCharacterMovementComponent::RefreshGait(const FAlsMovementGaitSettings* NewGaitSettings, const EAlsGait MaxAllowedGait)
{
	GaitSettings = *NewGaitSettings;

	SetCustomMaxWalkSpeed(GaitSettings.GetSpeedForGait(MaxAllowedGait));
}

float UAlsCharacterMovementComponent::CalculateGaitAmount() const
{
	// Map the character's current speed to the configured movement speeds ranging from 0 to 3,
	// where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting. This allows us to vary
	// the movement speeds but still use the mapped range in calculations for consistent results.

	const auto Speed{Velocity.Size2D()};

	if (Speed <= GaitSettings.WalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped({0, GaitSettings.WalkSpeed}, {0, 1}, Speed);
	}

	if (Speed <= GaitSettings.RunSpeed)
	{
		return FMath::GetMappedRangeValueClamped({GaitSettings.WalkSpeed, GaitSettings.RunSpeed}, {1, 2}, Speed);
	}

	return FMath::GetMappedRangeValueClamped({GaitSettings.RunSpeed, GaitSettings.SprintSpeed}, {2, 3}, Speed);
}

void UAlsCharacterMovementComponent::SetCustomMaxWalkSpeed(const float NewMaxWalkSpeed)
{
	if (CustomMaxWalkSpeed == NewMaxWalkSpeed)
	{
		return;
	}

	if (PawnOwner->IsLocallyControlled())
	{
		bMovementSettingsChangeRequested = true;
		CustomMaxWalkSpeed = NewMaxWalkSpeed;

		ServerSetCustomMaxWalkSpeed(NewMaxWalkSpeed);
		return;
	}

	if (!PawnOwner->HasAuthority())
	{
		MaxWalkSpeed = CustomMaxWalkSpeed;
		MaxWalkSpeedCrouched = CustomMaxWalkSpeed;
	}
}

void UAlsCharacterMovementComponent::ServerSetCustomMaxWalkSpeed_Implementation(const float NewMaxWalkSpeed)
{
	CustomMaxWalkSpeed = NewMaxWalkSpeed;
}
