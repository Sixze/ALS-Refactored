#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMovementCharacterSettings.h"

#include "AlsCharacterMovementComponent.generated.h"

class ALS_API FAlsSavedMove final : public FSavedMove_Character
{
private:
	using Super = FSavedMove_Character;

	bool bMovementSettingsChangeRequested{true};

protected:
	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* Character, float NewDeltaTime, const FVector& NewAcceleration,
	                        FNetworkPredictionData_Client_Character& Data) override;

	virtual uint8 GetCompressedFlags() const override;
};

class ALS_API FAlsNetworkPredictionData final : public FNetworkPredictionData_Client_Character
{
private:
	using Super = FNetworkPredictionData_Client_Character;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bMovementSettingsChangeRequested{true};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	float CustomMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsMovementGaitSettings GaitSettings;

public:
	UAlsCharacterMovementComponent();

	virtual float GetMaxAcceleration() const override;

	virtual float GetMaxBrakingDeceleration() const override;

protected:
	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;

	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;

public:
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

public:
	bool IsMovementSettingsChangeRequested() const;

	const FAlsMovementGaitSettings& GetGaitSettings() const;

	float GetCustomMaxWalkSpeed() const;

	void RefreshGait(const FAlsMovementGaitSettings* NewGaitSettings, EAlsGait MaxAllowedGait);

	float CalculateGaitAmount() const;

private:
	void SetCustomMaxWalkSpeed(float NewMaxWalkSpeed);

	UFUNCTION(Server, Reliable)
	void ServerSetCustomMaxWalkSpeed(float NewMaxWalkSpeed);
};

inline bool UAlsCharacterMovementComponent::IsMovementSettingsChangeRequested() const
{
	return bMovementSettingsChangeRequested;
}

inline const FAlsMovementGaitSettings& UAlsCharacterMovementComponent::GetGaitSettings() const
{
	return GaitSettings;
}

inline float UAlsCharacterMovementComponent::GetCustomMaxWalkSpeed() const
{
	return CustomMaxWalkSpeed;
}
