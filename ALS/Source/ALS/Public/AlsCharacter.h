#pragma once

#include "GameFramework/Character.h"
#include "Settings/AlsMantlingSettings.h"
#include "Settings/AlsMovementCharacterSettings.h"
#include "Settings/AlsRagdollingSettings.h"
#include "Settings/AlsRollingSettings.h"
#include "State/Enumerations/AlsGait.h"
#include "State/Enumerations/AlsLocomotionAction.h"
#include "State/Enumerations/AlsLocomotionMode.h"
#include "State/Enumerations/AlsOverlayMode.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"
#include "State/Structures/AlsAimingCharacterState.h"
#include "State/Structures/AlsInAirCharacterState.h"
#include "State/Structures/AlsLocomotionCharacterState.h"
#include "State/Structures/AlsMantlingState.h"
#include "State/Structures/AlsRagdollingCharacterState.h"
#include "State/Structures/AlsRollingState.h"

#include "AlsCharacter.generated.h"

enum class EAlsMantlingType : uint8;
class UAlsCharacterMovementComponent;
class UTimelineComponent;

UCLASS()
class ALS_API AAlsCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UAlsCharacterMovementComponent* AlsCharacterMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UTimelineComponent* MantlingTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess, ClampMin = 0))
	float MovingSpeedThreshold{150.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	bool bSprintHasPriorityOverAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	bool bRotateToVelocityWhenSprinting;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	bool bBlockAimingWhenInAir;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	bool bRotateToVelocityOnJump{true};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character",
		Meta = (AllowPrivateAccess, DisplayName = "Mantling Settings"))
	FAlsGeneralMantlingSettings GeneralMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	FAlsRagdollingSettings RagdollingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	FAlsRollingSettings RollingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Als Character", Meta = (AllowPrivateAccess))
	UAlsMovementCharacterSettings* MovementSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	EAlsStance DesiredStance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	EAlsGait DesiredGait{EAlsGait::Running};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsGait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	EAlsRotationMode DesiredRotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsRotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient,
		ReplicatedUsing = "OnReplicate_OverlayMode", Meta = (AllowPrivateAccess))
	EAlsOverlayMode OverlayMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsLocomotionMode LocomotionMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	EAlsLocomotionAction LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	FVector_NetQuantizeNormal InputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionCharacterState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	bool bDesiredAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	FRotator AimingRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsAimingCharacterState AimingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsInAirCharacterState InAirState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsMantlingState MantlingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated, Meta = (AllowPrivateAccess))
	FVector_NetQuantize RagdollTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsRagdollingCharacterState RagdollingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FAlsRollingState RollingState;

	FTimerHandle LandedGroundFrictionResetTimer;

	static const FCollisionObjectQueryParams MantlingAndRagdollObjectQueryParameters;

public:
	AAlsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void AddMovementInput(FVector Direction, float Scale = 1.0f, bool bForce = false) override;

	virtual void Jump() override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMode, uint8 PreviousCustomMode = 0) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnJumped_Implementation() override;

	virtual void Landed(const FHitResult& Hit) override;

	// Desired Stance

public:
	EAlsStance GetDesiredStance() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetDesiredStance(EAlsStance NewStance);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetDesiredStance(EAlsStance NewStance);

	void RefreshDesiredStance();

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

	// Overlay Mode

public:
	EAlsOverlayMode GetOverlayMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetOverlayMode(EAlsOverlayMode NewMode);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetOverlayMode(EAlsOverlayMode NewMode);

	UFUNCTION()
	void OnReplicate_OverlayMode(EAlsOverlayMode PreviousMode);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnOverlayModeChanged(EAlsOverlayMode PreviousMode);

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
	EAlsLocomotionAction GetLocomotionAction() const;

	void SetLocomotionAction(EAlsLocomotionAction NewAction);

	void NotifyLocomotionActionChanged(EAlsLocomotionAction PreviousAction);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnLocomotionActionChanged(EAlsLocomotionAction PreviousAction);

	// Locomotion

public:
	const FVector& GetInputDirection() const;

	const FAlsLocomotionCharacterState& GetLocomotionState() const;

private:
	void SetInputDirection(FVector NewInputDirection);

	FTransform CalculateSmoothTransform() const;

	void RefreshSmoothLocationAndRotation();

	void RefreshLocomotion(float DeltaTime);

	// Desired Aiming

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	void SetDesiredAiming(bool bNewDesiredAiming);

private:
	UFUNCTION(Server, Reliable)
	void ServerSetDesiredAiming(bool bNewDesiredAiming);

	// Aiming

public:
	const FRotator& GetAimingRotation() const;

private:
	void SetAimingRotation(const FRotator& NewAimingRotation);

public:
	const FAlsAimingCharacterState& GetAimingState() const;

private:
	void RefreshAiming(float DeltaTime);

	// Rotation

private:
	void RefreshGroundedActorRotation(float DeltaTime);

	void RefreshAimingActorRotation(float DeltaTime);

	void RefreshInAirActorRotation(float DeltaTime);

	float CalculateActorRotationSpeed() const;

	void RefreshActorRotation(const float TargetYawAngle, const float DeltaTime, const float RotationSpeed);

	void RefreshActorRotationExtraSmooth(const float TargetYawAngle, const float DeltaTime,
	                                     const float TargetRotationSpeed, const float ActorRotationSpeed);

	// Jumping

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnJumpedNetworked();

	void OnJumpedNetworked();

	// Landing

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnLandedNetworked();

	void OnLandedNetworked();

	void OnLandedGroundFrictionReset() const;

	// Mantling

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character")
	bool TryStartMantlingGrounded();

private:
	bool TryStartMantlingInAir();

	bool TryStartMantling(const FAlsMantlingTraceSettings& TraceSettings);

	UFUNCTION(Server, Reliable)
	void ServerStartMantling(const FAlsMantlingParameters& Parameters);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartMantling(const FAlsMantlingParameters& Parameters);

	void StartMantling(const FAlsMantlingParameters& Parameters);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	FAlsMantlingSettings SelectMantlingSettings(EAlsMantlingType MantlingType);

	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnMantlingStarted(const FAlsMantlingParameters& Parameters);

private:
	UFUNCTION()
	void OnMantlingTimelineUpdated(float BlendInTime);

	UFUNCTION()
	void OnMantlingTimelineEnded();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnMantlingEnded();

	// Ragdolling

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
	void StartRolling(float PlayRate, float TargetYawAngle);

	UFUNCTION(Server, Reliable)
	void ServerStartRolling(UAnimMontage* Montage, float PlayRate, float TargetYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRolling(UAnimMontage* Montage, float PlayRate, float TargetYawAngle);

	void StartRollingImplementation(UAnimMontage* Montage, float PlayRate, float TargetYawAngle);

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay,
	                          float& Unused, float& VerticalPosition) override;

private:
	static void DisplayDebugHeader(UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalPosition, float& VerticalPosition);

	void DisplayDebugCurves(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugState(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugShapes(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugTraces(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugMantling(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;
};

inline const FVector& AAlsCharacter::GetInputDirection() const
{
	return InputDirection;
}

inline const FAlsLocomotionCharacterState& AAlsCharacter::GetLocomotionState() const
{
	return LocomotionState;
}

inline const FRotator& AAlsCharacter::GetAimingRotation() const
{
	return AimingRotation;
}

inline const FAlsAimingCharacterState& AAlsCharacter::GetAimingState() const
{
	return AimingState;
}

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

inline EAlsRotationMode AAlsCharacter::GetDesiredRotationMode() const
{
	return DesiredRotationMode;
}

inline EAlsRotationMode AAlsCharacter::GetRotationMode() const
{
	return RotationMode;
}

inline EAlsOverlayMode AAlsCharacter::GetOverlayMode() const
{
	return OverlayMode;
}

inline EAlsLocomotionMode AAlsCharacter::GetLocomotionMode() const
{
	return LocomotionMode;
}

inline EAlsLocomotionAction AAlsCharacter::GetLocomotionAction() const
{
	return LocomotionAction;
}
