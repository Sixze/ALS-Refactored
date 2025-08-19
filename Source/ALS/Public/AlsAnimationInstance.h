#pragma once

#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "State/AlsControlRigInput.h"
#include "State/AlsCrouchingState.h"
#include "State/AlsDynamicTransitionsState.h"
#include "State/AlsFeetState.h"
#include "State/AlsGroundedState.h"
#include "State/AlsInAirState.h"
#include "State/AlsLayeringState.h"
#include "State/AlsLeanState.h"
#include "State/AlsLocomotionAnimationState.h"
#include "State/AlsLookState.h"
#include "State/AlsMovementBaseState.h"
#include "State/AlsPoseState.h"
#include "State/AlsRagdollingAnimationState.h"
#include "State/AlsRotateInPlaceState.h"
#include "State/AlsSpineState.h"
#include "State/AlsStandingState.h"
#include "State/AlsTransitionsState.h"
#include "State/AlsTurnInPlaceState.h"
#include "State/AlsViewAnimationState.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsAnimationInstance.generated.h"

class UAlsLinkedAnimationInstance;
class AAlsCharacter;

UCLASS()
class ALS_API UAlsAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

	friend UAlsLinkedAnimationInstance;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UAlsAnimationInstanceSettings> Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<AAlsCharacter> Character;

	// Used to indicate that the animation instance has not been updated for a long time
	// and its current state may not be correct (such as foot location used in foot lock).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bPendingUpdate : 1 {true};

	// Time of the last teleportation event.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 0))
	double TeleportedTime{0.0f};

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	uint8 bDisplayDebugTraces : 1 {false};

	mutable TArray<TFunction<void()>> DisplayDebugTracesQueue;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag ViewMode{AlsViewModeTags::ThirdPerson};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag LocomotionMode{AlsLocomotionModeTags::Grounded};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Stance{AlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Gait{AlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag OverlayMode{AlsOverlayModeTags::Default};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag LocomotionAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag GroundedEntryMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsMovementBaseState MovementBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLayeringState LayeringState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsPoseState PoseState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsViewAnimationState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsSpineState SpineState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLookState LookState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLocomotionAnimationState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsLeanState LeanState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsGroundedState GroundedState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsStandingState StandingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsCrouchingState CrouchingState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsInAirState InAirState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsFeetState FeetState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsTransitionsState TransitionsState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsDynamicTransitionsState DynamicTransitionsState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsRotateInPlaceState RotateInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsTurnInPlaceState TurnInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsRagdollingAnimationState RagdollingState;

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

	virtual void NativePostUpdateAnimation();

protected:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;

	// Core

protected:
	UFUNCTION(BlueprintPure, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe, ReturnDisplayName = "Setting"))
	UAlsAnimationInstanceSettings* GetSettingsUnsafe() const;

	UFUNCTION(BlueprintPure, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe, ReturnDisplayName = "Rig Input"))
	FAlsControlRigInput GetControlRigInput() const;

public:
	void MarkPendingUpdate();

	void MarkTeleported();

private:
	void RefreshMovementBaseOnGameThread();

	void RefreshLayering();

	void RefreshPose();

	// View

private:
	void RefreshViewOnGameThread();

	void RefreshView(float DeltaTime);

public:
	virtual bool IsSpineRotationAllowed();

private:
	void RefreshSpine(float SpineBlendAmount, float DeltaTime);

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeLook();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshLook();

	// Locomotion

private:
	void RefreshLocomotionOnGameThread();

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeLean();

	// Grounded

public:
	void SetGroundedEntryMode(const FGameplayTag& NewGroundedEntryMode);

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetGroundedEntryMode();

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeGrounded();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGrounded();

private:
	FVector3f GetRelativeVelocity() const;

	FVector2f GetRelativeAccelerationAmount() const;

	void RefreshVelocityBlend();

	void RefreshGroundedLean();

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshGroundedMovement();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void SetHipsDirection(EAlsHipsDirection NewHipsDirection);

private:
	void RefreshMovementDirection(float ViewRelativeVelocityYawAngle);

	void RefreshRotationYawOffsets(float ViewRelativeVelocityYawAngle);

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeStandingMovement();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshStandingMovement();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void ActivatePivot();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void ResetPivot();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshCrouchingMovement();

	// In Air

public:
	void Jump();

private:
	void RefreshInAirOnGameThread();

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshInAir();

	void RefreshGroundPrediction();

	void RefreshInAirLean();

	// Feet

private:
	void RefreshFeetOnGameThread();

	void RefreshFeet(float DeltaTime);

	void ProcessFootLockTeleport(const FAlsFootUpdateContext& Context) const;

	void ProcessFootLockBaseChange(const FAlsFootUpdateContext& Context) const;

	void RefreshFootLock(const FAlsFootUpdateContext& Context) const;

	// Transitions

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayQuickStopAnimation();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionAnimation(UAnimSequenceBase* Sequence, float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f,
	                             float PlayRate = 1.0f, float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionLeftAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f,
	                                 float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void PlayTransitionRightAnimation(float BlendInDuration = 0.2f, float BlendOutDuration = 0.2f, float PlayRate = 1.0f,
	                                  float StartTime = 0.0f, bool bFromStandingIdleOnly = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void StopTransitionAndTurnInPlaceAnimations(float BlendOutDuration = 0.2f);

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshDynamicTransitions();

private:
	void RefreshTransitions();

	void PlayQueuedTransitionAnimation();

	void StopQueuedTransitionAndTurnInPlaceAnimations();

	// Rotate In Place

public:
	virtual bool IsRotateInPlaceAllowed();

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshRotateInPlace();

	// Turn In Place

public:
	virtual bool IsTurnInPlaceAllowed();

protected:
	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void InitializeTurnInPlace();

	UFUNCTION(BlueprintCallable, Category = "ALS|Animation Instance", Meta = (BlueprintThreadSafe))
	void RefreshTurnInPlace();

private:
	void PlayQueuedTurnInPlaceAnimation();

	// Ragdolling

private:
	void RefreshRagdollingOnGameThread();

public:
	FPoseSnapshot& SnapshotFinalRagdollPose();

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

inline void UAlsAnimationInstance::MarkTeleported()
{
	TeleportedTime = GetWorld()->GetTimeSeconds();
}

inline void UAlsAnimationInstance::SetGroundedEntryMode(const FGameplayTag& NewGroundedEntryMode)
{
	GroundedEntryMode = NewGroundedEntryMode;
}

inline void UAlsAnimationInstance::ResetGroundedEntryMode()
{
	GroundedEntryMode = FGameplayTag::EmptyTag;
}

inline void UAlsAnimationInstance::SetHipsDirection(const EAlsHipsDirection NewHipsDirection)
{
	GroundedState.HipsDirection = NewHipsDirection;
}

inline void UAlsAnimationInstance::ResetPivot()
{
	StandingState.bPivotActive = false;
}

inline void UAlsAnimationInstance::Jump()
{
	InAirState.bJumpRequested = true;
}
