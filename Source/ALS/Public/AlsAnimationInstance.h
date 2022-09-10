#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "State/AlsFeetState.h"
#include "State/AlsGroundedState.h"
#include "State/AlsInAirState.h"
#include "State/AlsLayeringState.h"
#include "State/AlsLeanState.h"
#include "State/AlsLocomotionAnimationState.h"
#include "State/AlsPoseState.h"
#include "State/AlsRagdollingAnimationState.h"
#include "State/AlsRotateInPlaceState.h"
#include "State/AlsTransitionsState.h"
#include "State/AlsTurnInPlaceState.h"
#include "State/AlsViewAnimationState.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsAnimationInstance.generated.h"

class UAlsAnimationInstanceSettings;
class AAlsCharacter;

UCLASS()
class ALS_API UAlsAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UAlsAnimationInstanceSettings> Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<AAlsCharacter> Character;

	// Used to indicate that the animation instance has not been updated for a long time
	// and its current state may not be correct (such as foot location used in foot locking).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bPendingUpdate{true};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bTeleported;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bDisplayDebugTraces;

	TArray<TFunction<void()>> DisplayDebugTracesQueue;
#endif
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag ViewMode{AlsViewModeTags::ThirdPerson};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag LocomotionMode{AlsLocomotionModeTags::Grounded};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag RotationMode{AlsRotationModeTags::LookingDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Stance{AlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Gait{AlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag OverlayMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag GroundedEntryMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLayeringState LayeringState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsPoseState PoseState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsViewAnimationState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLeanState LeanState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLocomotionAnimationState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsGroundedState GroundedState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsInAirState InAirState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsFeetState FeetState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsTransitionsState TransitionsState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsRotateInPlaceState RotateInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsTurnInPlaceState TurnInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsRagdollingAnimationState RagdollingState;

public:
	UAlsAnimationInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

	virtual void NativePostEvaluateAnimation() override;

	// Core

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintProtected, BlueprintThreadSafe))
	UAlsAnimationInstanceSettings* GetSettingsUnsafe() const;

public:
	void MarkPendingUpdate();

private:
	void RefreshLayering();

	void RefreshPose();

	// View

public:
	virtual bool IsSpineRotationAllowed();

private:
	void RefreshViewGameThread();

	void RefreshView(float DeltaTime);

	void RefreshSpineRotation(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void ReinitializeLookTowardsInput();

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshLookTowardsInput(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void ReinitializeLookTowardsCamera();

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshLookTowardsCamera(float DeltaTime);

	// Locomotion

private:
	void RefreshLocomotionGameThread();

	// Grounded

public:
	void SetGroundedEntryMode(const FGameplayTag& NewModeTag);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetGroundedEntryMode();

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void SetHipsDirection(EAlsHipsDirection NewDirection);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void ActivatePivot();

private:
	void RefreshGroundedGameThread();

	void RefreshGrounded(float DeltaTime);

	void RefreshMovementDirection();

	void RefreshVelocityBlend(float DeltaTime);

	void RefreshRotationYawOffsets();

	void RefreshSprint(const FVector3f& RelativeAccelerationAmount, float DeltaTime);

	void RefreshStrideBlendAmount();

	void RefreshWalkRunBlendAmount();

	void RefreshStandingPlayRate();

	void RefreshCrouchingPlayRate();

	void RefreshGroundedLeanAmount(const FVector3f& RelativeAccelerationAmount, float DeltaTime);

	void ResetGroundedLeanAmount(float DeltaTime);

	// In Air

public:
	void Jump();

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetJumped();

private:
	void RefreshInAirGameThread();

	void RefreshInAir(float DeltaTime);

	void RefreshGroundPredictionAmount();

	void RefreshInAirLeanAmount(float DeltaTime);

	// Feet

private:
	void RefreshFeetGameThread();

	void RefreshFeet(float DeltaTime);

	void RefreshFoot(FAlsFootState& FootState, const FName& FootIkCurveName, const FName& FootLockCurveName,
	                 const FTransform& ComponentTransformInverse, float DeltaTime) const;

	void ProcessFootLockTeleport(FAlsFootState& FootState) const;

	void ProcessFootLockBaseChange(FAlsFootState& FootState, const FTransform& ComponentTransformInverse) const;

	void RefreshFootLock(FAlsFootState& FootState, const FName& FootLockCurveName, const FTransform& ComponentTransformInverse,
	                     float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const;

	void RefreshFootOffset(FAlsFootState& FootState, float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const;

	// Transitions

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayQuickStopAnimation();

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransitionAnimation(UAnimSequenceBase* Animation, float BlendInTime = 0.2f, float BlendOutTime = 0.2f,
	                             float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransitionLeftAnimation(float BlendInTime = 0.2f, float BlendOutTime = 0.2f, float PlayRate = 1.0f,
	                                 float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransitionRightAnimation(float BlendInTime = 0.2f, float BlendOutTime = 0.2f, float PlayRate = 1.0f,
	                                  float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void StopTransitionAndTurnInPlaceAnimations(float BlendOutTime = 0.2f);

private:
	void RefreshTransitions();

	void RefreshDynamicTransition();

	void PlayQueuedDynamicTransitionAnimation();

	// Rotate In Place

public:
	virtual bool IsRotateInPlaceAllowed();

private:
	void RefreshRotateInPlace(float DeltaTime);

	// Turn In Place

public:
	virtual bool IsTurnInPlaceAllowed();

private:
	void RefreshTurnInPlace(float DeltaTime);

	void PlayQueuedTurnInPlaceAnimation();

	// Ragdolling

private:
	void RefreshRagdollingGameThread();

public:
	void StopRagdolling();

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void FinalizeRagdolling() const;

	// Utility

public:
	float GetCurveValueClamped01(const FName& CurveName) const;
};

inline UAlsAnimationInstanceSettings* UAlsAnimationInstance::GetSettingsUnsafe() const
{
	return Settings;
}

inline void UAlsAnimationInstance::MarkPendingUpdate()
{
	bPendingUpdate |= true;
}

inline void UAlsAnimationInstance::SetGroundedEntryMode(const FGameplayTag& NewModeTag)
{
	GroundedEntryMode = NewModeTag;
}

inline void UAlsAnimationInstance::ResetGroundedEntryMode()
{
	GroundedEntryMode = FGameplayTag::EmptyTag;
}

inline void UAlsAnimationInstance::SetHipsDirection(const EAlsHipsDirection NewDirection)
{
	GroundedState.HipsDirection = NewDirection;
}

inline void UAlsAnimationInstance::ActivatePivot()
{
	GroundedState.bPivotActivationRequested = true;
}

inline void UAlsAnimationInstance::Jump()
{
	InAirState.bJumpRequested = true;
}

inline void UAlsAnimationInstance::ResetJumped()
{
	InAirState.bJumped = false;
}
