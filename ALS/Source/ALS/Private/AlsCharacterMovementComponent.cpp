#include "AlsCharacterMovementComponent.h"

#include "AlsCharacter.h"

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

void UAlsCharacterMovementComponent::SetCustomMaxWalkSpeed(const float NewMaxWalkSpeed)
{
	if (PawnOwner->IsLocallyControlled())
	{
		CustomMaxWalkSpeed = NewMaxWalkSpeed;
		ServerSetCustomMaxWalkSpeed(NewMaxWalkSpeed);
	}

	bMovementSettingsChangeRequested = true;
}

void UAlsCharacterMovementComponent::ServerSetCustomMaxWalkSpeed_Implementation(const float NewMaxWalkSpeed)
{
	CustomMaxWalkSpeed = NewMaxWalkSpeed;
}
