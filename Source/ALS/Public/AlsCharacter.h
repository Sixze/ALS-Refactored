#pragma once

#include "GameFramework/Character.h"
#include "State/AlsLocomotionState.h"
#include "State/AlsMantlingState.h"
#include "State/AlsMovementBaseState.h"
#include "State/AlsRagdollingState.h"
#include "State/AlsRollingState.h"
#include "State/AlsViewState.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsCharacter.generated.h"

struct FAlsMantlingParameters;
struct FAlsMantlingTraceSettings;
class UAlsCharacterMovementComponent;
class UAlsCharacterSettings;
class UAlsMovementSettings;
class UAlsAnimationInstance;
class UAlsMantlingSettings;

UCLASS(AutoExpandCategories = ("Settings|Als Character", "Settings|Als Character|Desired State"))
class ALS_API AAlsCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Als Character")
	TObjectPtr<UAlsCharacterMovementComponent> AlsCharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character")
	TObjectPtr<UAlsCharacterSettings> Settings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character")
	TObjectPtr<UAlsMovementSettings> MovementSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State",
		ReplicatedUsing = "OnReplicated_DesiredAiming")
	uint8 bDesiredAiming : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated)
	FGameplayTag DesiredRotationMode{AlsRotationModeTags::ViewDirection};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated)
	FGameplayTag DesiredStance{AlsStanceTags::Standing};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated)
	FGameplayTag DesiredGait{AlsGaitTags::Running};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", Replicated)
	FGameplayTag ViewMode{AlsViewModeTags::ThirdPerson};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State",
		ReplicatedUsing = "OnReplicated_OverlayMode")
	FGameplayTag OverlayMode{AlsOverlayModeTags::Default};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (ShowInnerProperties))
	TWeakObjectPtr<UAlsAnimationInstance> AnimationInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FGameplayTag LocomotionMode{AlsLocomotionModeTags::Grounded};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FGameplayTag Stance{AlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FGameplayTag Gait{AlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsMovementBaseState MovementBase;

	// Replicated raw view rotation. Depending on the context, this rotation can be in world space, or in movement
	// base space. In most cases, it is better to use FAlsViewState::Rotation to take advantage of network smoothing.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient,
		ReplicatedUsing = "OnReplicated_ReplicatedViewRotation")
	FRotator ReplicatedViewRotation{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsViewState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated)
	FVector_NetQuantizeNormal InputDirection{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character",
		Transient, Replicated, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float DesiredVelocityYawAngle{0.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	uint8 bHasDesiredVelocity : 1 {false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsLocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsMantlingState MantlingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated)
	FVector_NetQuantize RagdollTargetLocation{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsRagdollingState RagdollingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsRollingState RollingState;

	FTimerHandle BrakingFrictionFactorResetTimer;

public:
	explicit AAlsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* Property) const override;
#endif

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreRegisterAllComponents() override;

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

public:
	virtual void PostNetReceiveLocationAndRotation() override;

	virtual void OnRep_ReplicatedBasedMovement() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Restart() override;

public:
	const UAlsCharacterSettings* GetSettings() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character", Meta = (ReturnDisplayName = "Handled"))
	bool OnCalculateCamera(float DeltaTime, FMinimalViewInfo& ViewInfo);

private:
	void RefreshMeshProperties() const;

	void RefreshMovementBase();

	// View Mode

public:
	FGameplayTag GetViewMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewViewMode"))
	void SetViewMode(FGameplayTag NewViewMode);

private:
	void SetViewMode(FGameplayTag NewViewMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetViewMode(FGameplayTag NewViewMode);

	UFUNCTION(Server, Reliable)
	void ServerSetViewMode(FGameplayTag NewViewMode);

	// Locomotion Mode

public:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode = 0) override;

public:
	FGameplayTag GetLocomotionMode() const;

protected:
	void SetLocomotionMode(FGameplayTag NewLocomotionMode);

	virtual void NotifyLocomotionModeChanged(FGameplayTag PreviousLocomotionMode);

	UFUNCTION(BlueprintImplementableEvent, Category = "Als Character")
	void OnLocomotionModeChanged(FGameplayTag PreviousLocomotionMode);

	// Desired Aiming

public:
	bool IsDesiredAiming() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character")
	void SetDesiredAiming(bool bNewDesiredAiming);

private:
	void SetDesiredAiming(bool bNewDesiredAiming, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredAiming(bool bNewDesiredAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredAiming(bool bNewDesiredAiming);

	UFUNCTION()
	void OnReplicated_DesiredAiming(bool bPreviousDesiredAiming);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnDesiredAimingChanged(bool bPreviousDesiredAiming);

	// Desired Rotation Mode

public:
	FGameplayTag GetDesiredRotationMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredRotationMode"))
	void SetDesiredRotationMode(FGameplayTag NewDesiredRotationMode);

private:
	void SetDesiredRotationMode(FGameplayTag NewDesiredRotationMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredRotationMode(FGameplayTag NewDesiredRotationMode);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredRotationMode(FGameplayTag NewDesiredRotationMode);

	// Rotation Mode

public:
	FGameplayTag GetRotationMode() const;

protected:
	void SetRotationMode(FGameplayTag NewRotationMode);

	virtual void NotifyRotationModeChanged(FGameplayTag PreviousRotationMode);

	UFUNCTION(BlueprintImplementableEvent, Category = "Als Character")
	void OnRotationModeChanged(FGameplayTag PreviousRotationMode);

	void RefreshRotationMode();

	// Desired Stance

public:
	FGameplayTag GetDesiredStance() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredStance"))
	void SetDesiredStance(FGameplayTag NewDesiredStance);

private:
	void SetDesiredStance(FGameplayTag NewDesiredStance, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredStance(FGameplayTag NewDesiredStance);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredStance(FGameplayTag NewDesiredStance);

protected:
	virtual void ApplyDesiredStance();

	// Stance

public:
	virtual bool CanCrouch() const override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	FGameplayTag GetStance() const;

protected:
	void SetStance(FGameplayTag NewStance);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnStanceChanged(FGameplayTag PreviousStance);

	// Desired Gait

public:
	FGameplayTag GetDesiredGait() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredGait"))
	void SetDesiredGait(FGameplayTag NewDesiredGait);

private:
	void SetDesiredGait(FGameplayTag NewDesiredGait, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredGait(FGameplayTag NewDesiredGait);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredGait(FGameplayTag NewDesiredGait);

	// Gait

public:
	FGameplayTag GetGait() const;

protected:
	void SetGait(FGameplayTag NewGait);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnGaitChanged(FGameplayTag PreviousGait);

private:
	void RefreshGait();

	FGameplayTag CalculateMaxAllowedGait() const;

	FGameplayTag CalculateActualGait(FGameplayTag MaxAllowedGait) const;

	bool CanSprint() const;

	// Overlay Mode

public:
	FGameplayTag GetOverlayMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewOverlayMode"))
	void SetOverlayMode(FGameplayTag NewOverlayMode);

private:
	void SetOverlayMode(FGameplayTag NewOverlayMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetOverlayMode(FGameplayTag NewOverlayMode);

	UFUNCTION(Server, Reliable)
	void ServerSetOverlayMode(FGameplayTag NewOverlayMode);

	UFUNCTION()
	void OnReplicated_OverlayMode(FGameplayTag PreviousOverlayMode);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnOverlayModeChanged(FGameplayTag PreviousOverlayMode);

	// Locomotion Action

public:
	FGameplayTag GetLocomotionAction() const;

	void SetLocomotionAction(FGameplayTag NewLocomotionAction);

protected:
	virtual void NotifyLocomotionActionChanged(FGameplayTag PreviousLocomotionAction);

	UFUNCTION(BlueprintImplementableEvent, Category = "Als Character")
	void OnLocomotionActionChanged(FGameplayTag PreviousLocomotionAction);

	// Input

public:
	const FVector& GetInputDirection() const;

protected:
	void SetInputDirection(FVector NewInputDirection);

	virtual void RefreshInput(float DeltaTime);

	// View

public:
	virtual FRotator GetViewRotation() const override;

private:
	void SetReplicatedViewRotation(const FRotator& NewViewRotation, bool bSendRpc);

	UFUNCTION(Server, Unreliable)
	void ServerSetReplicatedViewRotation(const FRotator& NewViewRotation);

	UFUNCTION()
	void OnReplicated_ReplicatedViewRotation();

public:
	void CorrectViewNetworkSmoothing(const FRotator& NewTargetRotation, bool bRotationIsBaseRelative);

public:
	const FAlsViewState& GetViewState() const;

private:
	void RefreshView(float DeltaTime);

	void RefreshViewNetworkSmoothing(float DeltaTime);

	// Locomotion

public:
	const FAlsLocomotionState& GetLocomotionState() const;

private:
	void SetDesiredVelocityYawAngle(float NewVelocityYawAngle);

	void RefreshLocomotionEarly();

	void RefreshLocomotion();

	void RefreshLocomotionLate();

	UFUNCTION(Server, Reliable)
	void ServerSetInitialVelocityYawAngle(float NewVelocityYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetInitialVelocityYawAngle(float NewVelocityYawAngle);

	// Jumping

public:
	virtual void Jump() override;

	virtual void OnJumped_Implementation() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnJumpedNetworked();

	void OnJumpedNetworked();

	// Rotation

public:
	virtual void FaceRotation(FRotator Rotation, float DeltaTime) override final;

	void CharacterMovement_OnPhysicsRotation(float DeltaTime);

private:
	void RefreshGroundedRotation(float DeltaTime);

protected:
	virtual bool RefreshCustomGroundedMovingRotation(float DeltaTime);

	virtual bool RefreshCustomGroundedNotMovingRotation(float DeltaTime);

	float CalculateGroundedMovingRotationInterpolationHalfLife() const;

	void RefreshGroundedAimingRotation(float DeltaTime);

	bool ConstrainAimingRotation(FRotator& ActorRotation, float DeltaTime, bool bApplySecondaryConstraint = false);

private:
	void ApplyRotationYawSpeedAnimationCurve(float DeltaTime);

	void RefreshInAirRotation(float DeltaTime);

protected:
	virtual bool RefreshCustomInAirRotation(float DeltaTime);

	void RefreshInAirAimingRotation(float DeltaTime);

	void SetRotationSmooth(float TargetYawAngle, float DeltaTime, float InterpolationHalfLife);

	void SetRotationExtraSmooth(float TargetYawAngle, float DeltaTime, float InterpolationHalfLife, float TargetYawAngleRotationSpeed);

	void SetRotationInstant(float TargetYawAngle, ETeleportType Teleport = ETeleportType::None);

	void RefreshTargetYawAngleUsingActorRotation();

	void SetTargetYawAngle(float TargetYawAngle);

	void SetTargetYawAngleSmooth(float TargetYawAngle, float DeltaTime, float RotationSpeed);

	void RefreshViewRelativeTargetYawAngle();

	// Rolling

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Character")
	void StartRolling(float PlayRate = 1.0f);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	UAnimMontage* SelectRollMontage();

	bool IsRollingAllowedToStart(const UAnimMontage* Montage) const;

private:
	void StartRolling(float PlayRate, float TargetYawAngle);

	UFUNCTION(Server, Reliable)
	void ServerStartRolling(UAnimMontage* Montage, float PlayRate, float InitialYawAngle, float TargetYawAngle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRolling(UAnimMontage* Montage, float PlayRate, float InitialYawAngle, float TargetYawAngle);

	void StartRollingImplementation(UAnimMontage* Montage, float PlayRate, float InitialYawAngle, float TargetYawAngle);

	void RefreshRolling(float DeltaTime);

	void RefreshRollingPhysics(float DeltaTime);

	// Mantling

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	bool IsMantlingAllowedToStart() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (ReturnDisplayName = "Success"))
	bool StartMantlingGrounded();

private:
	bool StartMantlingInAir();

	bool StartMantling(const FAlsMantlingTraceSettings& TraceSettings);

	UFUNCTION(Server, Reliable)
	void ServerStartMantling(const FAlsMantlingParameters& Parameters);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartMantling(const FAlsMantlingParameters& Parameters);

	void StartMantlingImplementation(const FAlsMantlingParameters& Parameters);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	UAlsMantlingSettings* SelectMantlingSettings(EAlsMantlingType MantlingType);

	float CalculateMantlingStartTime(const UAlsMantlingSettings* MantlingSettings, float MantlingHeight) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnMantlingStarted(const FAlsMantlingParameters& Parameters);

private:
	void RefreshMantling();

	void StopMantling(bool bStopMontage = false);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnMantlingEnded();

	// Ragdolling

public:
	const FAlsRagdollingState& GetRagdollingState() const;

	bool IsRagdollingAllowedToStart() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character")
	void StartRagdolling();

private:
	UFUNCTION(Server, Reliable)
	void ServerStartRagdolling();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRagdolling();

	void StartRagdollingImplementation();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnRagdollingStarted();

public:
	bool IsRagdollingAllowedToStop() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (ReturnDisplayName = "Success"))
	bool StopRagdolling();

private:
	UFUNCTION(Server, Reliable)
	void ServerStopRagdolling();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopRagdolling();

	void StopRagdollingImplementation();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	UAnimMontage* SelectGetUpMontage(bool bRagdollFacingUpward);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnRagdollingEnded();

private:
	void SetRagdollTargetLocation(const FVector& NewTargetLocation);

	UFUNCTION(Server, Unreliable)
	void ServerSetRagdollTargetLocation(const FVector_NetQuantize& NewTargetLocation);

	void RefreshRagdolling(float DeltaTime);

	FVector RagdollTraceGround(bool& bGrounded) const;

	void ConstraintRagdollSpeed() const;

	// Footsteps

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Als Character")
	void SpawnFootstepSound(const float VolumeMultiplier, const float PitchMultiplier);
	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

private:
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, FLinearColor HeaderColor,
	                               float Scale, float HorizontalLocation, float& VerticalLocation);

	void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugState(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugMantling(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;
};

inline const UAlsCharacterSettings* AAlsCharacter::GetSettings() const
{
	return Settings;
}

inline FGameplayTag AAlsCharacter::GetViewMode() const
{
	return ViewMode;
}

inline FGameplayTag AAlsCharacter::GetLocomotionMode() const
{
	return LocomotionMode;
}

inline bool AAlsCharacter::IsDesiredAiming() const
{
	return bDesiredAiming;
}

inline FGameplayTag AAlsCharacter::GetDesiredRotationMode() const
{
	return DesiredRotationMode;
}

inline FGameplayTag AAlsCharacter::GetRotationMode() const
{
	return RotationMode;
}

inline FGameplayTag AAlsCharacter::GetDesiredStance() const
{
	return DesiredStance;
}

inline FGameplayTag AAlsCharacter::GetStance() const
{
	return Stance;
}

inline FGameplayTag AAlsCharacter::GetDesiredGait() const
{
	return DesiredGait;
}

inline FGameplayTag AAlsCharacter::GetGait() const
{
	return Gait;
}

inline FGameplayTag AAlsCharacter::GetOverlayMode() const
{
	return OverlayMode;
}

inline FGameplayTag AAlsCharacter::GetLocomotionAction() const
{
	return LocomotionAction;
}

inline const FVector& AAlsCharacter::GetInputDirection() const
{
	return InputDirection;
}

inline const FAlsViewState& AAlsCharacter::GetViewState() const
{
	return ViewState;
}

inline const FAlsLocomotionState& AAlsCharacter::GetLocomotionState() const
{
	return LocomotionState;
}

inline const FAlsRagdollingState& AAlsCharacter::GetRagdollingState() const
{
	return RagdollingState;
}
