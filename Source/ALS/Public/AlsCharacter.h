#pragma once

#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Settings/AlsMantlingSettings.h"
#include "State/Enumerations/AlsGait.h"
#include "State/Enumerations/AlsLocomotionMode.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"
#include "State/Enumerations/AlsViewMode.h"
#include "State/Structures/AlsLocomotionState.h"
#include "State/Structures/AlsRagdollingState.h"
#include "State/Structures/AlsViewState.h"

#include "AlsCharacter.generated.h"

class UAlsCharacterMovementComponent;
class UAlsCharacterSettings;
class UAlsMovementSettings;
class UAlsAnimationInstance;

UCLASS()
class ALS_API AAlsCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UAlsCharacterMovementComponent* AlsCharacterMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	UAlsCharacterSettings* Settings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	UAlsMovementSettings* MovementSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated, Meta = (AllowPrivateAccess))
	EAlsStance DesiredStance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated, Meta = (AllowPrivateAccess))
	EAlsGait DesiredGait{EAlsGait::Running};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State",
		ReplicatedUsing = "OnReplicate_DesiredAiming", Meta = (AllowPrivateAccess))
	bool bDesiredAiming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated, Meta = (AllowPrivateAccess))
	EAlsRotationMode DesiredRotationMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated, Meta = (AllowPrivateAccess))
	EAlsViewMode ViewMode{EAlsViewMode::ThirdPerson};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State",
		ReplicatedUsing = "OnReplicate_OverlayMode", Meta = (AllowPrivateAccess))
	FGameplayTag OverlayMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Instanced, Meta = (AllowPrivateAccess))
	UAlsAnimationInstance* AlsAnimationInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsGait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsRotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsLocomotionMode LocomotionMode{EAlsLocomotionMode::Grounded};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	FVector_NetQuantizeNormal InputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	FRotator ViewRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsViewState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	int32 MantlingRootMotionSourceId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	FVector_NetQuantize RagdollTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsRagdollingState RagdollingState;

	FTimerHandle LandedGroundFrictionResetTimer;

public:
	AAlsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:
	virtual void Restart() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Jump() override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMode, uint8 PreviousCustomMode = 0) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnJumped_Implementation() override;

private:
	void PhysicsRotation(float DeltaTime);

	// Desired Stance

public:
	EAlsStance GetDesiredStance() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetDesiredStance(EAlsStance NewStance);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetDesiredStance(EAlsStance NewStance);

	void ApplyDesiredStance();

	// Stance

public:
	EAlsStance GetStance() const;

private:
	void SetStance(EAlsStance NewStance);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnStanceChanged(EAlsStance PreviousStance);

	// Desired Gait

public:
	EAlsGait GetDesiredGait() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetDesiredGait(EAlsGait NewGait);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetDesiredGait(EAlsGait NewGait);

	// Gait

public:
	EAlsGait GetGait() const;

private:
	void SetGait(EAlsGait NewGait);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnGaitChanged(EAlsGait PreviousGait);

private:
	void RefreshGait();

	EAlsGait CalculateMaxAllowedGait() const;

	EAlsGait CalculateActualGait(EAlsGait MaxAllowedGait) const;

	bool CanSprint() const;

	// Desired Aiming

public:
	bool IsDesiredAiming() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetDesiredAiming(bool bNewDesiredAiming);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetDesiredAiming(bool bNewDesiredAiming);

	UFUNCTION()
	void OnReplicate_DesiredAiming(bool bPreviousDesiredAiming);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnDesiredAimingChanged(bool bPreviousDesiredAiming);

	// Desired Rotation Mode

public:
	EAlsRotationMode GetDesiredRotationMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetDesiredRotationMode(EAlsRotationMode NewMode);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetDesiredRotationMode(EAlsRotationMode NewMode);

	// Rotation Mode

public:
	EAlsRotationMode GetRotationMode() const;

private:
	void SetRotationMode(EAlsRotationMode NewMode);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnRotationModeChanged(EAlsRotationMode PreviousMode);

	void RefreshRotationMode();

	// View Mode

public:
	EAlsViewMode GetViewMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetViewMode(EAlsViewMode NewMode);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetViewMode(EAlsViewMode NewMode);

	// Overlay Mode

public:
	FGameplayTag GetOverlayMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character", Meta = (AutoCreateRefTerm = "NewModeTag"))
	void SetOverlayMode(const FGameplayTag& NewModeTag);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetOverlayMode(const FGameplayTag& NewModeTag);

	UFUNCTION()
	void OnReplicate_OverlayMode(const FGameplayTag& PreviousModeTag);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnOverlayModeChanged(const FGameplayTag& PreviousModeTag);

	// Locomotion Mode

public:
	EAlsLocomotionMode GetLocomotionMode() const;

private:
	void SetLocomotionMode(EAlsLocomotionMode NewMode);

	void NotifyLocomotionModeChanged(EAlsLocomotionMode PreviousMode);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnLocomotionModeChanged(EAlsLocomotionMode PreviousMode);

	// Locomotion Action

public:
	FGameplayTag GetLocomotionAction() const;

	void SetLocomotionAction(const FGameplayTag& NewActionTag);

	void NotifyLocomotionActionChanged(const FGameplayTag& PreviousActionTag);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnLocomotionActionChanged(const FGameplayTag& PreviousActionTag);

	// Locomotion

public:
	const FVector& GetInputDirection() const;

	const FAlsLocomotionState& GetLocomotionState() const;

private:
	void SetInputDirection(FVector NewInputDirection);

	FTransform CalculateNetworkSmoothedTransform() const;

	void RefreshLocomotionLocationAndRotation();

	void RefreshLocomotion(float DeltaTime);

	// View

public:
	virtual FRotator GetViewRotation() const override;

private:
	void SetViewRotation(const FRotator& NewViewRotation);

	UFUNCTION(Server, Unreliable)
	void ServerSetViewRotation(const FRotator& NewViewRotation);

public:
	const FAlsViewState& GetViewState() const;

private:
	void RefreshView(float DeltaTime);

	// Rotation

private:
	void RefreshGroundedActorRotation(float DeltaTime);

protected:
	virtual bool TryRefreshCustomGroundedMovingActorRotation(float DeltaTime);

	virtual bool TryRefreshCustomGroundedNotMovingActorRotation(float DeltaTime);

	void RefreshGroundedMovingAimingActorRotation(float DeltaTime);

	void RefreshGroundedNotMovingAimingActorRotation(float DeltaTime);

	float CalculateActorRotationSpeed() const;

private:
	void ApplyRotationYawSpeed(float DeltaTime);

	void ResetRotationYawSpeed();

	void RefreshInAirActorRotation(float DeltaTime);

protected:
	virtual bool TryRefreshCustomInAirActorRotation(float DeltaTime);

	void RefreshInAirAimingActorRotation(float DeltaTime);

protected:
	void RefreshTargetYawAngle(float TargetYawAngle, bool bRefreshSmoothTargetYawAngle = true);

	void RefreshActorRotationInstant(float TargetYawAngle, ETeleportType Teleport = ETeleportType::None);

	void RefreshActorRotation(float TargetYawAngle, float DeltaTime, float ActorRotationInterpolationSpeed);

	void RefreshActorRotationExtraSmooth(float TargetYawAngle, float DeltaTime,
	                                     float ActorRotationInterpolationSpeed,
	                                     float TargetYawAngleRotationSpeed);

	// Rotation Lock

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void LockRotation(float TargetYawAngle);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void UnLockRotation();

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLockRotation(float TargetYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUnLockRotation();

	// Jumping

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnJumpedNetworked();

	void OnJumpedNetworked();

	// Landing

private:
	void OnLandedGroundFrictionReset() const;

	// Mantling

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	bool TryStartMantlingGrounded();

private:
	bool TryStartMantlingInAir();

	bool IsMantlingAllowedToStart() const;

	bool TryStartMantling(const FAlsMantlingTraceSettings& TraceSettings);

	UFUNCTION(Server, Reliable)
	void ServerStartMantling(const FAlsMantlingParameters& Parameters);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartMantling(const FAlsMantlingParameters& Parameters);

	void StartMantlingImplementation(const FAlsMantlingParameters& Parameters);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	UAlsMantlingSettings* SelectMantlingSettings(EAlsMantlingType MantlingType);

	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnMantlingStarted(const FAlsMantlingParameters& Parameters);

private:
	void RefreshMantling();

	void StopMantling();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnMantlingEnded();

	// Ragdolling

private:
	bool IsRagdollingAllowedToStart() const;

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void StartRagdolling();

private:
	UFUNCTION(Server, Reliable)
	void ServerStartRagdolling();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRagdolling();

	void StartRagdollingImplementation();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnRagdollingStarted();

private:
	bool IsRagdollingAllowedToStop() const;

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	bool TryStopRagdolling();

private:
	UFUNCTION(Server, Reliable)
	void ServerStopRagdolling();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopRagdolling();

	void StopRagdollingImplementation();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	UAnimMontage* SelectGetUpMontage(bool bRagdollFacedUpward);

	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnRagdollingEnded();

private:
	void SetRagdollTargetLocation(const FVector& NewLocation);

	UFUNCTION(Server, Unreliable)
	void ServerSetRagdollTargetLocation(const FVector& NewLocation);

	void RefreshRagdolling(float DeltaTime);

	void RefreshRagdollingActorTransform(float DeltaTime);

	// Rolling

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void TryStartRolling(float PlayRate = 1.0f);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	UAnimMontage* SelectRollMontage();

private:
	bool IsRollingAllowedToStart(const UAnimMontage* Montage) const;

	void StartRolling(float PlayRate, float TargetYawAngle);

	UFUNCTION(Server, Reliable)
	void ServerStartRolling(UAnimMontage* Montage, float PlayRate, float StartYawAngle, float TargetYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRolling(UAnimMontage* Montage, float PlayRate, float StartYawAngle, float TargetYawAngle);

	void StartRollingImplementation(UAnimMontage* Montage, float PlayRate, float StartYawAngle, float TargetYawAngle);

	void RefreshRollingPhysics(float DeltaTime);

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalPosition) override;

private:
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalPosition, float& VerticalPosition);

	void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugState(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugMantling(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;
};

inline EAlsStance AAlsCharacter::GetDesiredStance() const
{
	return DesiredStance;
}

inline EAlsStance AAlsCharacter::GetStance() const
{
	return Stance;
}

inline EAlsGait AAlsCharacter::GetDesiredGait() const
{
	return DesiredGait;
}

inline EAlsGait AAlsCharacter::GetGait() const
{
	return Gait;
}

inline bool AAlsCharacter::IsDesiredAiming() const
{
	return bDesiredAiming;
}

inline EAlsRotationMode AAlsCharacter::GetDesiredRotationMode() const
{
	return DesiredRotationMode;
}

inline EAlsRotationMode AAlsCharacter::GetRotationMode() const
{
	return RotationMode;
}

inline EAlsViewMode AAlsCharacter::GetViewMode() const
{
	return ViewMode;
}

inline FGameplayTag AAlsCharacter::GetOverlayMode() const
{
	return OverlayMode;
}

inline EAlsLocomotionMode AAlsCharacter::GetLocomotionMode() const
{
	return LocomotionMode;
}

inline FGameplayTag AAlsCharacter::GetLocomotionAction() const
{
	return LocomotionAction;
}

inline const FVector& AAlsCharacter::GetInputDirection() const
{
	return InputDirection;
}

inline const FAlsLocomotionState& AAlsCharacter::GetLocomotionState() const
{
	return LocomotionState;
}

inline const FAlsViewState& AAlsCharacter::GetViewState() const
{
	return ViewState;
}
