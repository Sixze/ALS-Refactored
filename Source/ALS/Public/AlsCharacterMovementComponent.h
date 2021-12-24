#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMovementSettings.h"

#include "AlsCharacterMovementComponent.generated.h"

using FAlsPhysicsRotationDelegate = TMulticastDelegate<void(float DeltaTime)>;

class ALS_API FAlsCharacterNetworkMoveData : public FCharacterNetworkMoveData
{
private:
	using Super = FCharacterNetworkMoveData;

public:
	EAlsStance Stance{EAlsStance::Standing};

	EAlsRotationMode RotationMode{EAlsRotationMode::LookingDirection};

	EAlsGait MaxAllowedGait{EAlsGait::Walking};

public:
	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Archive, UPackageMap* Map, ENetworkMoveType MoveType) override;
};

class ALS_API FAlsCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:
	FAlsCharacterNetworkMoveData MoveData[3];

public:
	FAlsCharacterNetworkMoveDataContainer();
};

class ALS_API FAlsSavedMove : public FSavedMove_Character
{
private:
	using Super = FSavedMove_Character;

public:
	EAlsStance Stance{EAlsStance::Standing};

	EAlsRotationMode RotationMode{EAlsRotationMode::LookingDirection};

	EAlsGait MaxAllowedGait{EAlsGait::Walking};

public:
	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* Character, float NewDeltaTime, const FVector& NewAcceleration,
	                        FNetworkPredictionData_Client_Character& PredictionData) override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;

	virtual void PrepMoveFor(ACharacter* Character) override;
};

class ALS_API FAlsNetworkPredictionData : public FNetworkPredictionData_Client_Character
{
private:
	using Super = FNetworkPredictionData_Client_Character;

public:
	FAlsNetworkPredictionData(const UCharacterMovementComponent& MovementComponent);

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class ALS_API UAlsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend FAlsSavedMove;

private:
	FAlsCharacterNetworkMoveDataContainer MoveDataContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	UAlsMovementSettings* MovementSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsMovementGaitSettings GaitSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	EAlsStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	EAlsRotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	EAlsGait MaxAllowedGait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bMovementModeLocked;

public:
	FAlsPhysicsRotationDelegate OnPhysicsRotation;

public:
	UAlsCharacterMovementComponent();

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* Property) const override;
#endif

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode) override;

protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxAcceleration() const override;

	virtual float GetMaxBrakingDeceleration() const override;

	virtual void PhysicsRotation(float DeltaTime) override;

protected:
	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAcceleration) override;

public:
	void SetMovementSettings(UAlsMovementSettings* NewMovementSettings);

	const FAlsMovementGaitSettings& GetGaitSettings() const;

private:
	void RefreshGaitSettings();

public:
	void SetStance(EAlsStance NewStance);

	void SetRotationMode(EAlsRotationMode NewMode);

	void SetMaxAllowedGait(EAlsGait NewGait);

private:
	void RefreshMaxWalkSpeed();

public:
	float CalculateGaitAmount() const;

	void SetMovementModeLocked(bool bNewMovementModeLocked);
};

inline const FAlsMovementGaitSettings& UAlsCharacterMovementComponent::GetGaitSettings() const
{
	return GaitSettings;
}
