#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "State/Enumerations/AlsGait.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"
#include "State/Enumerations/AlsViewMode.h"
#include "State/Structures/AlsFeetState.h"
#include "State/Structures/AlsGroundedState.h"
#include "State/Structures/AlsInAirState.h"
#include "State/Structures/AlsLayeringState.h"
#include "State/Structures/AlsLeanState.h"
#include "State/Structures/AlsLocomotionAnimationState.h"
#include "State/Structures/AlsPoseState.h"
#include "State/Structures/AlsRagdollingAnimationState.h"
#include "State/Structures/AlsRotateInPlaceState.h"
#include "State/Structures/AlsTransitionsState.h"
#include "State/Structures/AlsTurnInPlaceState.h"
#include "State/Structures/AlsViewAnimationState.h"
#include "AlsAnimationInstance.generated.h"

class UAlsAnimationInstanceSettings;
class AAlsCharacter;

UCLASS()
class ALS_API UAlsAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	UAlsAnimationInstanceSettings* Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	AAlsCharacter* Character;

	// Used to indicate that the animation instance has not been updated for a long time
	// and its current state may be incorrect (such as foot location used in foot locking).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bPendingUpdate{true};

	// The animation curves will be relevant if the character is rendered or VisibilityBasedAnimTickOption
	// is set to AlwaysTickPoseAndRefreshBones, otherwise the curves will have their old values even though
	// the animation blueprint continues to update.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bAnimationCurvesRelevant;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bJustTeleported;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsStance Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsGait Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRotationMode RotationMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsViewMode ViewMode{EAlsViewMode::ThirdPerson};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag GroundedEntryMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLayeringState LayeringState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsPoseState PoseState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsViewAnimationState ViewState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLeanState LeanState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsLocomotionAnimationState LocomotionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsGroundedState GroundedState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsInAirState InAirState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsFeetState FeetState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsTransitionsState TransitionsState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRotateInPlaceState RotateInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsTurnInPlaceState TurnInPlaceState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsRagdollingAnimationState RagdollingState;

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	// Core

public:
	void SetPendingUpdate(bool bNewPendingUpdate);

	void SetAnimationCurvesRelevant(bool bNewRelevant);

	const FAlsFeetState& GetFeetState() const;

private:
	void RefreshLayering();

	void RefreshPose();

	// View

private:
	void RefreshView(float DeltaTime);

protected:
	virtual bool IsSpineRotationAllowed();

	// Locomotion

private:
	void RefreshLocomotion(float DeltaTime);

	// Grounded

public:
	void SetGroundedEntryMode(const FGameplayTag& NewModeTag);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void ResetGroundedEntryMode();

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void SetHipsDirection(EAlsHipsDirection NewDirection);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void ActivatePivot();

private:
	void RefreshGrounded(float DeltaTime);

	void RefreshMovementDirection();

	void RefreshVelocityBlend(const float DeltaTime);

	void RefreshRotationYawOffsets();

	float CalculateStrideBlendAmount() const;

	float CalculateWalkRunBlendAmount() const;

	float CalculateStandingPlayRate() const;

	float CalculateCrouchingPlayRate() const;

	// In Air

public:
	void Jump();

private:
	void RefreshInAir(float DeltaTime);

	float CalculateGroundPredictionAmount() const;

	FAlsLeanState CalculateInAirLeanAmount() const;

	// Feet

private:
	void RefreshFeet(float DeltaTime);

	void ProcessFootLockTeleport(FAlsFootState& FootState, const FTransform& CapsuleTransform) const;

	void ProcessFootLockBaseChange(FAlsFootState& FootState, const FName& FootBoneName, const FVector& BaseLocation,
	                               const FQuat& BaseRotation, const FTransform& CapsuleRelativeTransform) const;

	void RefreshFootLock(FAlsFootState& FootState, const FName& FootBoneName,
	                     const FName& FootLockCurveName, const FTransform& CapsuleRelativeTransform,
	                     float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const;

	void RefreshFootOffset(FAlsFootState& FootState, float DeltaTime, FVector& TargetLocationOffset,
	                       FVector& FinalLocation, FQuat& FinalRotation) const;

	void ResetFootOffset(FAlsFootState& FootState, float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const;

	static void RefreshFinalFootState(FAlsFootState& FootState, const FTransform& MeshRelativeTransform,
	                                  const FVector& FinalLocation, const FQuat& FinalRotation);

	void RefreshPelvisOffset(float DeltaTime, float TargetFootLeftLocationOffsetZ, float TargetFootRightLocationOffsetZ);

	// Transitions

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	UAnimSequenceBase* SelectDynamicTransitionForLeftFoot() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	UAnimSequenceBase* SelectDynamicTransitionForRightFoot() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransition(UAnimSequenceBase* Animation, float BlendInTime = 0.2f, float BlendOutTime = 0.2f,
	                    float PlayRate = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void PlayTransitionFromStandingIdleOnly(UAnimSequenceBase* Animation, float BlendInTime = 0.2f,
	                                        float BlendOutTime = 0.2f, float PlayRate = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void StopTransitionAndTurnInPlaceAnimations(float BlendOutTime = 0.2f);

private:
	void RefreshTransitions();

	void PlayDynamicTransition(UAnimSequenceBase* Animation, float BlendInTime = 0.2f, float BlendOutTime = 0.2f,
	                           float PlayRate = 1.0f, float StartTime = 0.0f, float AllowanceDelay = 0.0f);

	// Rotate In Place

protected:
	virtual bool IsRotateInPlaceAllowed();

private:
	void RefreshRotateInPlace(float DeltaTime);

	// Turn In Place

protected:
	virtual bool IsTurnInPlaceAllowed();

private:
	void RefreshTurnInPlace(float DeltaTime);

	void StartTurnInPlace(float TargetYawAngle, float PlayRateScale = 1.0f, float StartTime = 0.0f, bool bAllowRestartIfPlaying = false);

	// Ragdolling

private:
	void RefreshRagdolling();

public:
	void StopRagdolling();

private:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Animation Instance")
	void FinalizeRagdolling();

	// Utility

public:
	float GetCurveValueClamped01(const FName& CurveName) const;
};

inline void UAlsAnimationInstance::SetPendingUpdate(const bool bNewPendingUpdate)
{
	bPendingUpdate = bNewPendingUpdate;
}

inline void UAlsAnimationInstance::SetAnimationCurvesRelevant(const bool bNewRelevant)
{
	bAnimationCurvesRelevant = bNewRelevant;
}

inline const FAlsFeetState& UAlsAnimationInstance::GetFeetState() const
{
	return FeetState;
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
