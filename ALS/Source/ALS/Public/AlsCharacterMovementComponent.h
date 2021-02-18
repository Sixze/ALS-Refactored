#pragma once

#include "GameFramework/CharacterMovementComponent.h"

#include "AlsCharacterMovementComponent.generated.h"

class ALS_API FAlsSavedMove final : public FSavedMove_Character
{
private:
	typedef FSavedMove_Character Super;

	bool bMovementSettingsChangeRequested{true};

protected:
	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* Character, float NewDeltaTime, FVector const& NewAcceleration,
	                        FNetworkPredictionData_Client_Character& Data) override;

	virtual uint8 GetCompressedFlags() const override;
};

class ALS_API FAlsNetworkPredictionData final : public FNetworkPredictionData_Client_Character
{
private:
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FAlsNetworkPredictionData(const UCharacterMovementComponent& MovementComponent);

protected:
	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class ALS_API UAlsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

private:
	bool bMovementSettingsChangeRequested{true};

	float CustomMaxWalkSpeed;

public:
	UAlsCharacterMovementComponent();

	bool IsMovementSettingsChangeRequested() const;

	float GetCustomMaxWalkSpeed() const;

	void SetCustomMaxWalkSpeed(float NewMaxWalkSpeed);

protected:
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

private:
	UFUNCTION(Server, Reliable)
	void ServerSetCustomMaxWalkSpeed(float NewMaxWalkSpeed);
};

inline bool UAlsCharacterMovementComponent::IsMovementSettingsChangeRequested() const
{
	return bMovementSettingsChangeRequested;
}

inline float UAlsCharacterMovementComponent::GetCustomMaxWalkSpeed() const
{
	return CustomMaxWalkSpeed;
}
