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
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	FGameplayTag Stance{AlsStanceTags::Standing};

	FGameplayTag MaxAllowedGait{AlsGaitTags::Running};

public:
	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Archive, UPackageMap* Map, ENetworkMoveType MoveType) override;
};

class ALS_API FAlsCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:
	TStaticArray<FAlsCharacterNetworkMoveData, 3> MoveData;

public:
	FAlsCharacterNetworkMoveDataContainer();
};

class ALS_API FAlsSavedMove : public FSavedMove_Character
{
private:
	using Super = FSavedMove_Character;

public:
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	FGameplayTag Stance{AlsStanceTags::Standing};

	FGameplayTag MaxAllowedGait{AlsGaitTags::Running};

public:
	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* Character, float NewDeltaTime, const FVector& NewAcceleration,
	                        FNetworkPredictionData_Client_Character& PredictionData) override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDeltaTime) const override;

	virtual void CombineWith(const FSavedMove_Character* PreviousMove, ACharacter* Character,
	                         APlayerController* Player, const FVector& PreviousStartLocation) override;

	virtual void PrepMoveFor(ACharacter* Character) override;
};

class ALS_API FAlsNetworkPredictionData : public FNetworkPredictionData_Client_Character
{
private:
	using Super = FNetworkPredictionData_Client_Character;

public:
	explicit FAlsNetworkPredictionData(const UCharacterMovementComponent& Movement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS(ClassGroup = "ALS")
class ALS_API UAlsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend FAlsSavedMove;

public:
	// If checked, improves the response to interaction from moving kinematic physical
	// bodies, but may cause some issues when interacting with simulated physical bodies.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	uint8 bAllowImprovedPenetrationAdjustment : 1 {true};

protected:
	FAlsCharacterNetworkMoveDataContainer MoveDataContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<UAlsMovementSettings> MovementSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsMovementGaitSettings GaitSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Stance{AlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag MaxAllowedGait{AlsGaitTags::Running};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 0, ClampMax = 3))
	float GaitAmount{0.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 0, ForceUnits = "cm/s^2"))
	float MaxAccelerationWalking{0.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bMovementModeLocked : 1 {false};

	// Used to temporarily prohibit the player from moving the character. Also works for AI-controlled characters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bInputBlocked : 1 {false};

	// Valid only on locally controlled characters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FRotator PreviousControlRotation{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PrePenetrationAdjustmentVelocity{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bPrePenetrationAdjustmentVelocityValid : 1 {false};

public:
	FAlsPhysicsRotationDelegate OnPhysicsRotation;

public:
	UAlsCharacterMovementComponent();

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* Property) const override;
#endif

	virtual void BeginPlay() override;

	virtual FVector ConsumeInputVector() override;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual bool ShouldPerformAirControlForPathFollowing() const override;

	virtual void UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation) override;

	virtual bool ApplyRequestedMove(float DeltaTime, float CurrentMaxAcceleration, float MaxSpeed, float Friction,
	                                float BrakingDeceleration, FVector& RequestedAcceleration, float& RequestedSpeed) override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	virtual float GetMaxAcceleration() const override;

protected:
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaTime) override;

public:
	virtual void PhysicsRotation(float DeltaTime) override;

	// ReSharper disable once CppRedefinitionOfDefaultArgumentInOverrideFunction
	virtual void MoveSmooth(const FVector& InVelocity, float DeltaTime, FStepDownResult* StepDownResult = nullptr) override;

protected:
	virtual void PhysWalking(float DeltaTime, int32 IterationsCount) override;

	virtual void PhysNavWalking(float DeltaTime, int32 IterationsCount) override;

	virtual void PhysCustom(float DeltaTime, int32 IterationsCount) override;

public:
	virtual void ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult,
	                              float SweepRadius, const FHitResult* DownwardSweepResult) const override;

protected:
	virtual void PerformMovement(float DeltaTime) override;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void SmoothClientPosition(float DeltaTime) override;

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAcceleration) override;

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Character Movement")
	void SetMovementSettings(UAlsMovementSettings* NewMovementSettings);

	const FAlsMovementGaitSettings& GetGaitSettings() const;

private:
	void RefreshGaitSettings();

public:
	const FGameplayTag& GetRotationMode() const;

	void SetRotationMode(const FGameplayTag& NewRotationMode);

	const FGameplayTag& GetStance() const;

	void SetStance(const FGameplayTag& NewStance);

	const FGameplayTag& GetMaxAllowedGait() const;

	void SetMaxAllowedGait(const FGameplayTag& NewMaxAllowedGait);

	// Returns the character's current speed, mapped to the speed ranges from the movement settings.
	// Varies from 0 to 3, where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting.
	float GetGaitAmount() const;

private:
	void RefreshGroundedMovementSettings();

public:
	void SetMovementModeLocked(bool bNewMovementModeLocked);

	void SetInputBlocked(bool bNewInputBlocked);

	bool TryConsumePrePenetrationAdjustmentVelocity(FVector& OutVelocity);
};

inline const FAlsMovementGaitSettings& UAlsCharacterMovementComponent::GetGaitSettings() const
{
	return GaitSettings;
}

inline const FGameplayTag& UAlsCharacterMovementComponent::GetRotationMode() const
{
	return RotationMode;
}

inline const FGameplayTag& UAlsCharacterMovementComponent::GetStance() const
{
	return Stance;
}

inline const FGameplayTag& UAlsCharacterMovementComponent::GetMaxAllowedGait() const
{
	return MaxAllowedGait;
}

inline void UAlsCharacterMovementComponent::SetMaxAllowedGait(const FGameplayTag& NewMaxAllowedGait)
{
	MaxAllowedGait = NewMaxAllowedGait;
}

inline float UAlsCharacterMovementComponent::GetGaitAmount() const
{
	return GaitAmount;
}
