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

UCLASS(AutoExpandCategories = ("Settings|Als Character", "Settings|Als Character|Desired State", "State|Als Character"))
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
	uint8 bDesiredAiming : 1;

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
	FRotator ReplicatedViewRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsViewState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated)
	FVector_NetQuantizeNormal InputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character",
		Transient, Replicated, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float DesiredVelocityYawAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsLocomotionState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient)
	FAlsMantlingState MantlingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Replicated)
	FVector_NetQuantize RagdollTargetLocation;

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

	virtual void PostRegisterAllComponents() override;

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostNetReceiveLocationAndRotation() override;

	virtual void OnRep_ReplicatedBasedMovement() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Restart() override;

private:
	void RefreshMeshProperties() const;

	void RefreshMovementBase();

	// View Mode

public:
	const FGameplayTag& GetViewMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewViewMode"))
	void SetViewMode(const FGameplayTag& NewViewMode);

private:
	void SetViewMode(const FGameplayTag& NewViewMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetViewMode(const FGameplayTag& NewViewMode);

	UFUNCTION(Server, Reliable)
	void ServerSetViewMode(const FGameplayTag& NewViewMode);

	// Locomotion Mode

public:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode = 0) override;

public:
	const FGameplayTag& GetLocomotionMode() const;

protected:
	void SetLocomotionMode(const FGameplayTag& NewLocomotionMode);

	virtual void NotifyLocomotionModeChanged(const FGameplayTag& PreviousLocomotionMode);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnLocomotionModeChanged(const FGameplayTag& PreviousLocomotionMode);

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
	const FGameplayTag& GetDesiredRotationMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredRotationMode"))
	void SetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode);

private:
	void SetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredRotationMode(const FGameplayTag& NewDesiredRotationMode);

	// Rotation Mode

public:
	const FGameplayTag& GetRotationMode() const;

protected:
	void SetRotationMode(const FGameplayTag& NewRotationMode);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnRotationModeChanged(const FGameplayTag& PreviousRotationMode);

	void RefreshRotationMode();

	// Desired Stance

public:
	const FGameplayTag& GetDesiredStance() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredStance"))
	void SetDesiredStance(const FGameplayTag& NewDesiredStance);

private:
	void SetDesiredStance(const FGameplayTag& NewDesiredStance, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredStance(const FGameplayTag& NewDesiredStance);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredStance(const FGameplayTag& NewDesiredStance);

protected:
	virtual void ApplyDesiredStance();

	// Stance

public:
	virtual bool CanCrouch() const override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
	const FGameplayTag& GetStance() const;

protected:
	void SetStance(const FGameplayTag& NewStance);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnStanceChanged(const FGameplayTag& PreviousStance);

	// Desired Gait

public:
	const FGameplayTag& GetDesiredGait() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewDesiredGait"))
	void SetDesiredGait(const FGameplayTag& NewDesiredGait);

private:
	void SetDesiredGait(const FGameplayTag& NewDesiredGait, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetDesiredGait(const FGameplayTag& NewDesiredGait);

	UFUNCTION(Server, Reliable)
	void ServerSetDesiredGait(const FGameplayTag& NewDesiredGait);

	// Gait

public:
	const FGameplayTag& GetGait() const;

protected:
	void SetGait(const FGameplayTag& NewGait);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnGaitChanged(const FGameplayTag& PreviousGait);

private:
	void RefreshGait();

	FGameplayTag CalculateMaxAllowedGait() const;

	FGameplayTag CalculateActualGait(const FGameplayTag& MaxAllowedGait) const;

	bool CanSprint() const;

	// Overlay Mode

public:
	const FGameplayTag& GetOverlayMode() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Character", Meta = (AutoCreateRefTerm = "NewOverlayMode"))
	void SetOverlayMode(const FGameplayTag& NewOverlayMode);

private:
	void SetOverlayMode(const FGameplayTag& NewOverlayMode, bool bSendRpc);

	UFUNCTION(Client, Reliable)
	void ClientSetOverlayMode(const FGameplayTag& NewOverlayMode);

	UFUNCTION(Server, Reliable)
	void ServerSetOverlayMode(const FGameplayTag& NewOverlayMode);

	UFUNCTION()
	void OnReplicated_OverlayMode(const FGameplayTag& PreviousOverlayMode);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnOverlayModeChanged(const FGameplayTag& PreviousOverlayMode);

	// Locomotion Action

public:
	const FGameplayTag& GetLocomotionAction() const;

	void SetLocomotionAction(const FGameplayTag& NewLocomotionAction);

protected:
	virtual void NotifyLocomotionActionChanged(const FGameplayTag& PreviousLocomotionAction);

	UFUNCTION(BlueprintNativeEvent, Category = "Als Character")
	void OnLocomotionActionChanged(const FGameplayTag& PreviousLocomotionAction);

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
	void CorrectViewNetworkSmoothing(const FRotator& NewTargetRotation, bool bRelativeTargetRotation);

public:
	const FAlsViewState& GetViewState() const;

private:
	void RefreshView(float DeltaTime);

	void RefreshViewNetworkSmoothing(float DeltaTime);

	// Locomotion

public:
	const FAlsLocomotionState& GetLocomotionState() const;

private:
	void SetDesiredVelocityYawAngle(float NewDesiredVelocityYawAngle);

	void RefreshLocomotionLocationAndRotation();

	void RefreshLocomotionEarly();

	void RefreshLocomotion(float DeltaTime);

	void RefreshLocomotionLate(float DeltaTime);

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

	float CalculateGroundedMovingRotationInterpolationSpeed() const;

	void RefreshGroundedAimingRotation(float DeltaTime);

	bool RefreshConstrainedAimingRotation(float DeltaTime, bool bApplySecondaryConstraint = false);

private:
	void ApplyRotationYawSpeedAnimationCurve(float DeltaTime);

	void RefreshInAirRotation(float DeltaTime);

protected:
	virtual bool RefreshCustomInAirRotation(float DeltaTime);

	void RefreshInAirAimingRotation(float DeltaTime);

	void RefreshRotation(float TargetYawAngle, float DeltaTime, float RotationInterpolationSpeed);

	void RefreshRotationExtraSmooth(float TargetYawAngle, float DeltaTime,
	                                float RotationInterpolationSpeed, float TargetYawAngleRotationSpeed);

	void RefreshRotationInstant(float TargetYawAngle, ETeleportType Teleport = ETeleportType::None);

	void RefreshTargetYawAngleUsingLocomotionRotation();

	void RefreshTargetYawAngle(float TargetYawAngle);

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

	void LimitRagdollSpeed() const;

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

private:
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalLocation, float& VerticalLocation);

	void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugState(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugMantling(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;
};

inline const FGameplayTag& AAlsCharacter::GetViewMode() const
{
	return ViewMode;
}

inline const FGameplayTag& AAlsCharacter::GetLocomotionMode() const
{
	return LocomotionMode;
}

inline bool AAlsCharacter::IsDesiredAiming() const
{
	return bDesiredAiming;
}

inline const FGameplayTag& AAlsCharacter::GetDesiredRotationMode() const
{
	return DesiredRotationMode;
}

inline const FGameplayTag& AAlsCharacter::GetRotationMode() const
{
	return RotationMode;
}

inline const FGameplayTag& AAlsCharacter::GetDesiredStance() const
{
	return DesiredStance;
}

inline const FGameplayTag& AAlsCharacter::GetStance() const
{
	return Stance;
}

inline const FGameplayTag& AAlsCharacter::GetDesiredGait() const
{
	return DesiredGait;
}

inline const FGameplayTag& AAlsCharacter::GetGait() const
{
	return Gait;
}

inline const FGameplayTag& AAlsCharacter::GetOverlayMode() const
{
	return OverlayMode;
}

inline const FGameplayTag& AAlsCharacter::GetLocomotionAction() const
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

