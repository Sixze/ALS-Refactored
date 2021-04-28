#pragma once

#include "Animation/AnimInstance.h"
#include "Settings//AlsGeneralSettings.h"
#include "Settings/AlsDynamicTransitionSettings.h"
#include "Settings/AlsFeetSettings.h"
#include "Settings/AlsInAirSettings.h"
#include "Settings/AlsMovementAnimationSettings.h"
#include "Settings/AlsRotateInPlaceSettings.h"
#include "Settings/AlsTurnInPlaceSettings.h"
#include "State/Enumerations/AlsGait.h"
#include "State/Enumerations/AlsGroundedEntryMode.h"
#include "State/Enumerations/AlsLocomotionAction.h"
#include "State/Enumerations/AlsLocomotionMode.h"
#include "State/Enumerations/AlsMovementDirection.h"
#include "State/Enumerations/AlsOverlayMode.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"
#include "State/Structures/AlsAimingAnimationState.h"
#include "State/Structures/AlsFeetState.h"
#include "State/Structures/AlsInAirAnimationState.h"
#include "State/Structures/AlsLayeringState.h"
#include "State/Structures/AlsLeanState.h"
#include "State/Structures/AlsLocomotionAnimationState.h"
#include "State/Structures/AlsMovementState.h"
#include "State/Structures/AlsRagdollingAnimationState.h"
#include "State/Structures/AlsRotateInPlaceState.h"
#include "State/Structures/AlsTurnInPlaceState.h"
#include "Utility/AlsConstants.h"

#include "AlsAnimationInstance.generated.h"

class AAlsCharacter;

UCLASS()
class ALS_API UAlsAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings", Transient, Meta = (AllowPrivateAccess))
	FAlsPropertyAccessConstants Constants;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsGeneralSettings GeneralSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsFeetSettings FeetSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsMovementAnimationSettings MovementSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsRotateInPlaceSettings RotateInPlaceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsGeneralTurnInPlaceSettings TurnInPlaceSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsDynamicTransitionSettings DynamicTransitionSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsInAirSettings InAirSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	AAlsCharacter* AlsCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bDynamicTransitionsAllowed{true};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsGait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsOverlayMode OverlayMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionMode LocomotionMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionAction LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsMovementDirection MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsGroundedEntryMode GroundedEntryMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionAnimationState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLayeringState LayeringState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsAimingAnimationState AimingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsFeetState FeetState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLeanState LeanState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRotateInPlaceState RotateInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsTurnInPlaceState TurnInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsInAirAnimationState InAirState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRagdollingAnimationState RagdollingState;

	FTimerHandle DynamicTransitionsAllowanceTimer;

	FTimerHandle PivotResetTimer;

	FTimerHandle JumpResetTimer;

	static const FCollisionObjectQueryParams GroundPredictionObjectQueryParameters;

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	// Core

private:
	void RefreshLocomotion(float DeltaTime);

	void RefreshLayering();

	void RefreshAiming(float DeltaTime);

	// Feet

private:
	void RefreshFeet(float DeltaTime);

	void RefreshFootLock(FAlsFootState& FootState, const FName& FootBoneName, const FName& FootLockCurveName, float DeltaTime) const;

	void RefreshFootOffset(FAlsFootState& FootState, FVector& TargetLocationOffset, float DeltaTime) const;

	static void ResetFootOffset(FAlsFootState& FootState, float DeltaTime);

	void RefreshPelvisOffset(float DeltaTime, const FVector& TargetFootLeftLocationOffset, const FVector& TargetFootRightLocationOffset);

	float CalculateFootIkRootScaleAmount() const;

	// Grounded Movement

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void SetGroundedEntryMode(EAlsGroundedEntryMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void SetHipsDirection(EAlsHipsDirection NewDirection);

private:
	EAlsMovementDirection CalculateMovementDirection() const;

	void RefreshMovement(float DeltaTime);

	void RefreshVelocityBlend(const float DeltaTime);

	float CalculateStrideBlendAmount() const;

	float CalculateWalkRunBlendAmount() const;

	float CalculateStandingPlayRate() const;

	float CalculateCrouchingPlayRate() const;

	// Pivot

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void ActivatePivot();

private:
	void OnPivotResetTimerEnded();

	// Transitions

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransition(UAnimSequenceBase* Animation, float BlendInTime = 0.2f, float BlendOutTime = 0.2f,
	                    float PlayRate = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransitionFromStandingIdleOnly(UAnimSequenceBase* Animation, float BlendInTime = 0.2f, float BlendOutTime = 0.2f,
	                                        float PlayRate = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void StopTransitionAndTurnInPlaceSlotAnimations(float BlendOutTime = 0.2f);

private:
	void RefreshDynamicTransitions();

	void PlayDynamicTransition(UAnimSequenceBase* Animation, float BlendInTime = 0.2f, float BlendOutTime = 0.2f,
	                           float PlayRate = 1.0f, float StartTime = 0.0f, float AllowanceDelayTime = 0.0f);

	void OnDynamicTransitionAllowanceTimerEnded();

	// Rotate In Place

private:
	void RefreshRotateInPlace();

	// Turn In Place

private:
	void RefreshTurnInPlace(float DeltaTime);

	void StartTurnInPlace(float TargetYawAngle, float PlayRateScale = 1.0f, float StartTime = 0.0f, bool bAllowRestartIfPlaying = false);

	// Jump

public:
	void Jump();

private:
	void OnJumpResetTimerEnded();

	// In Air

private:
	void RefreshInAir(float DeltaTime);

	float CalculateGroundPredictionAmount() const;

	FAlsLeanState CalculateInAirLeanAmount() const;

	// Ragdolling

private:
	void RefreshRagdolling();

	void StopRagdolling();

	// Utility

public:
	float GetCurveValueClamped01(const FName& CurveName) const;
};

inline void UAlsAnimationInstance::SetGroundedEntryMode(const EAlsGroundedEntryMode NewMode)
{
	GroundedEntryMode = NewMode;
}

inline void UAlsAnimationInstance::SetHipsDirection(const EAlsHipsDirection NewDirection)
{
	MovementState.HipsDirection = NewDirection;
}
