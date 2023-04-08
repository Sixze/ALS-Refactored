#include "AlsAnimationInstance.h"

#include "AlsAnimationInstanceProxy.h"
#include "AlsCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsAnimationInstanceSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationInstance)

UAlsAnimationInstance::UAlsAnimationInstance()
{
	RootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;
}

void UAlsAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AAlsCharacter>(GetOwningActor());

#if WITH_EDITOR
	if (!GetWorld()->IsGameWorld() && !IsValid(Character))
	{
		// Use default objects for editor preview.

		Character = GetMutableDefault<AAlsCharacter>();
	}
#endif
}

void UAlsAnimationInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ALS_ENSURE(IsValid(Settings));
	ALS_ENSURE(IsValid(Character));
}

void UAlsAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::NativeUpdateAnimation()"),
	                            STAT_UAlsAnimationInstance_NativeUpdateAnimation, STATGROUP_Als)

	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	// AAlsCharacter::FinalizeRagdolling() should only be called from here, not from AAlsCharacter::StopRagdollingImplementation(),
	// otherwise the character mesh can sometimes take a strange pose when transitioning from ragdoll states to animation states.

	Character->FinalizeRagdolling();

	if (GetSkelMeshComponent()->IsUsingAbsoluteRotation())
	{
		const auto& ActorTransform{Character->GetActorTransform()};

		// Manually synchronize mesh rotation with character rotation.

		GetSkelMeshComponent()->MoveComponent(
			FVector::ZeroVector, ActorTransform.GetRotation() * Character->GetBaseRotationOffset(), false);

		// Re-cache proxy transforms to match the modified mesh transform.

		const auto& Proxy{GetProxyOnGameThread<FAnimInstanceProxy>()};

		const_cast<FTransform&>(Proxy.GetComponentTransform()) = GetSkelMeshComponent()->GetComponentTransform();
		const_cast<FTransform&>(Proxy.GetComponentRelativeTransform()) = GetSkelMeshComponent()->GetRelativeTransform();
		const_cast<FTransform&>(Proxy.GetActorTransform()) = ActorTransform;
	}

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	bDisplayDebugTraces = UAlsUtility::ShouldDisplayDebugForActor(Character, UAlsConstants::TracesDebugDisplayName());
#endif

	ViewMode = Character->GetViewMode();
	LocomotionMode = Character->GetLocomotionMode();
	RotationMode = Character->GetRotationMode();
	Stance = Character->GetStance();
	Gait = Character->GetGait();
	OverlayMode = Character->GetOverlayMode();

	if (LocomotionAction != Character->GetLocomotionAction())
	{
		LocomotionAction = Character->GetLocomotionAction();

		ResetGroundedEntryMode();
	}

	RefreshMovementBaseOnGameThread();
	RefreshViewOnGameThread();
	RefreshLocomotionOnGameThread();
	RefreshGroundedOnGameThread();
	RefreshInAirOnGameThread();

	RefreshFeetOnGameThread();

	RefreshRagdollingOnGameThread();
}

void UAlsAnimationInstance::NativeThreadSafeUpdateAnimation(const float DeltaTime)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::NativeThreadSafeUpdateAnimation()"),
	                            STAT_UAlsAnimationInstance_NativeThreadSafeUpdateAnimation, STATGROUP_Als)

	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (!IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	RefreshLayering();
	RefreshPose();

	RefreshView(DeltaTime);
	RefreshGrounded(DeltaTime);
	RefreshInAir(DeltaTime);

	RefreshFeet(DeltaTime);

	RefreshTransitions();
	RefreshRotateInPlace(DeltaTime);
	RefreshTurnInPlace(DeltaTime);
}

void UAlsAnimationInstance::NativePostEvaluateAnimation()
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::NativePostEvaluateAnimation()"),
	                            STAT_UAlsAnimationInstance_NativePostEvaluateAnimation, STATGROUP_Als)

	Super::NativePostEvaluateAnimation();

	if (!IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	PlayQueuedDynamicTransitionAnimation();
	PlayQueuedTurnInPlaceAnimation();

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (!bPendingUpdate)
	{
		for (const auto& DisplayDebugTraceFunction : DisplayDebugTracesQueue)
		{
			DisplayDebugTraceFunction();
		}
	}

	DisplayDebugTracesQueue.Reset();
#endif

	bPendingUpdate = false;
}

FAnimInstanceProxy* UAlsAnimationInstance::CreateAnimInstanceProxy()
{
	return new FAlsAnimationInstanceProxy{this};
}

FAlsControlRigInput UAlsAnimationInstance::GetControlRigInput() const
{
	return {
		!IsValid(Settings) || Settings->General.bUseHandIkBones,
		!IsValid(Settings) || Settings->General.bUseFootIkBones,
		GroundedState.VelocityBlend.ForwardAmount,
		GroundedState.VelocityBlend.BackwardAmount,
		ViewState.SpineRotation.YawAngle,
		FeetState.Left.IkRotation,
		FeetState.Left.IkLocation,
		FeetState.Left.IkAmount,
		FeetState.Right.IkRotation,
		FeetState.Right.IkLocation,
		FeetState.Right.IkAmount,
		FeetState.MinMaxPelvisOffsetZ,
	};
}

void UAlsAnimationInstance::RefreshMovementBaseOnGameThread()
{
	const auto& BasedMovement{Character->GetBasedMovement()};

	if (BasedMovement.MovementBase != MovementBase.Primitive || BasedMovement.BoneName != MovementBase.BoneName)
	{
		MovementBase.Primitive = BasedMovement.MovementBase;
		MovementBase.BoneName = BasedMovement.BoneName;
		MovementBase.bBaseChanged = true;
	}
	else
	{
		MovementBase.bBaseChanged = false;
	}

	MovementBase.bHasRelativeLocation = BasedMovement.HasRelativeLocation();
	MovementBase.bHasRelativeRotation = MovementBase.bHasRelativeLocation && BasedMovement.bRelativeRotation;

	const auto PreviousRotation{MovementBase.Rotation};

	MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName,
	                                              MovementBase.Location, MovementBase.Rotation);

	MovementBase.DeltaRotation = MovementBase.bHasRelativeLocation && !MovementBase.bBaseChanged
		                             ? (MovementBase.Rotation * PreviousRotation.Inverse()).Rotator()
		                             : FRotator::ZeroRotator;
}

void UAlsAnimationInstance::RefreshLayering()
{
	const auto& Curves{GetProxyOnAnyThread<FAlsAnimationInstanceProxy>().GetAnimationCurves(EAnimCurveType::AttributeCurve)};

	static const auto GetCurveValue{
		[](const TMap<FName, float>& Curves, const FName& CurveName) -> float
		{
			const auto* Value{Curves.Find(CurveName)};

			return Value != nullptr ? *Value : 0.0f;
		}
	};

	LayeringState.HeadBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerHeadCurveName());
	LayeringState.HeadAdditiveBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerHeadAdditiveCurveName());
	LayeringState.HeadSlotBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerHeadSlotCurveName());

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmLeftBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmLeftCurveName());
	LayeringState.ArmLeftAdditiveBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmLeftAdditiveCurveName());
	LayeringState.ArmLeftSlotBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmLeftSlotCurveName());
	LayeringState.ArmLeftLocalSpaceBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmLeftLocalSpaceCurveName());
	LayeringState.ArmLeftMeshSpaceBlendAmount = !FAnimWeight::IsFullWeight(LayeringState.ArmLeftLocalSpaceBlendAmount);

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmRightBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmRightCurveName());
	LayeringState.ArmRightAdditiveBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmRightAdditiveCurveName());
	LayeringState.ArmRightSlotBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmRightSlotCurveName());
	LayeringState.ArmRightLocalSpaceBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerArmRightLocalSpaceCurveName());
	LayeringState.ArmRightMeshSpaceBlendAmount = !FAnimWeight::IsFullWeight(LayeringState.ArmRightLocalSpaceBlendAmount);

	LayeringState.HandLeftBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerHandLeftCurveName());
	LayeringState.HandRightBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerHandRightCurveName());

	LayeringState.SpineBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerSpineCurveName());
	LayeringState.SpineAdditiveBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerSpineAdditiveCurveName());
	LayeringState.SpineSlotBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerSpineSlotCurveName());

	LayeringState.PelvisBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerPelvisCurveName());
	LayeringState.PelvisSlotBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerPelvisSlotCurveName());

	LayeringState.LegsBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerLegsCurveName());
	LayeringState.LegsSlotBlendAmount = GetCurveValue(Curves, UAlsConstants::LayerLegsSlotCurveName());
}

void UAlsAnimationInstance::RefreshPose()
{
	const auto& Curves{GetProxyOnAnyThread<FAlsAnimationInstanceProxy>().GetAnimationCurves(EAnimCurveType::AttributeCurve)};

	static const auto GetCurveValue{
		[](const TMap<FName, float>& Curves, const FName& CurveName) -> float
		{
			const auto* Value{Curves.Find(CurveName)};

			return Value != nullptr ? *Value : 0.0f;
		}
	};

	PoseState.GroundedAmount = GetCurveValue(Curves, UAlsConstants::PoseGroundedCurveName());
	PoseState.InAirAmount = GetCurveValue(Curves, UAlsConstants::PoseInAirCurveName());

	PoseState.StandingAmount = GetCurveValue(Curves, UAlsConstants::PoseStandingCurveName());
	PoseState.CrouchingAmount = GetCurveValue(Curves, UAlsConstants::PoseCrouchingCurveName());

	PoseState.MovingAmount = GetCurveValue(Curves, UAlsConstants::PoseMovingCurveName());

	PoseState.GaitAmount = FMath::Clamp(GetCurveValue(Curves, UAlsConstants::PoseGaitCurveName()), 0.0f, 3.0f);
	PoseState.GaitWalkingAmount = UAlsMath::Clamp01(PoseState.GaitAmount);
	PoseState.GaitRunningAmount = UAlsMath::Clamp01(PoseState.GaitAmount - 1.0f);
	PoseState.GaitSprintingAmount = UAlsMath::Clamp01(PoseState.GaitAmount - 2.0f);

	// Use the grounded pose curve value to "unweight" the gait pose curve. This is used to
	// instantly get the full gait value from the very beginning of transitions to grounded states.

	PoseState.UnweightedGaitAmount = PoseState.GroundedAmount > 0.0f
		                                 ? PoseState.GaitAmount / PoseState.GroundedAmount
		                                 : PoseState.GaitAmount;

	PoseState.UnweightedGaitWalkingAmount = UAlsMath::Clamp01(PoseState.UnweightedGaitAmount);
	PoseState.UnweightedGaitRunningAmount = UAlsMath::Clamp01(PoseState.UnweightedGaitAmount - 1.0f);
	PoseState.UnweightedGaitSprintingAmount = UAlsMath::Clamp01(PoseState.UnweightedGaitAmount - 2.0f);
}

void UAlsAnimationInstance::RefreshViewOnGameThread()
{
	check(IsInGameThread())

	const auto& View{Character->GetViewState()};

	ViewState.Rotation = View.Rotation;
	ViewState.YawSpeed = View.YawSpeed;
}

bool UAlsAnimationInstance::IsSpineRotationAllowed()
{
	return RotationMode == AlsRotationModeTags::Aiming;
}

void UAlsAnimationInstance::RefreshView(const float DeltaTime)
{
	if (!LocomotionAction.IsValid())
	{
		ViewState.YawAngle = FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw - LocomotionState.Rotation.Yaw));
		ViewState.PitchAngle = FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT(ViewState.Rotation.Pitch - LocomotionState.Rotation.Pitch));

		ViewState.PitchAmount = 0.5f - ViewState.PitchAngle / 180.0f;
	}

	const auto ViewAmount{1.0f - GetCurveValueClamped01(UAlsConstants::ViewBlockCurveName())};
	const auto AimingAmount{GetCurveValueClamped01(UAlsConstants::AllowAimingCurveName())};

	ViewState.LookAmount = ViewAmount * (1.0f - AimingAmount);

	RefreshSpineRotation(DeltaTime);

	ViewState.SpineRotation.YawAngle *= ViewAmount * AimingAmount;
}

void UAlsAnimationInstance::RefreshSpineRotation(const float DeltaTime)
{
	auto& SpineRotation{ViewState.SpineRotation};

	if (SpineRotation.bSpineRotationAllowed != IsSpineRotationAllowed())
	{
		SpineRotation.bSpineRotationAllowed = !SpineRotation.bSpineRotationAllowed;
		SpineRotation.StartYawAngle = SpineRotation.CurrentYawAngle;
	}

	if (SpineRotation.bSpineRotationAllowed)
	{
		static constexpr auto InterpolationSpeed{20.0f};

		SpineRotation.SpineAmount = bPendingUpdate
			                            ? 1.0f
			                            : UAlsMath::ExponentialDecay(SpineRotation.SpineAmount, 1.0f, DeltaTime, InterpolationSpeed);

		SpineRotation.TargetYawAngle = ViewState.YawAngle;
	}
	else
	{
		static constexpr auto InterpolationSpeed{10.0f};

		SpineRotation.SpineAmount = bPendingUpdate
			                            ? 0.0f
			                            : UAlsMath::ExponentialDecay(SpineRotation.SpineAmount, 0.0f, DeltaTime, InterpolationSpeed);
	}

	SpineRotation.CurrentYawAngle = UAlsMath::LerpAngle(SpineRotation.StartYawAngle, SpineRotation.TargetYawAngle,
	                                                    SpineRotation.SpineAmount);

	SpineRotation.YawAngle = SpineRotation.CurrentYawAngle;
}

void UAlsAnimationInstance::ReinitializeLookTowardsInput()
{
	ViewState.LookTowardsInput.bReinitializationRequired = true;
}

void UAlsAnimationInstance::RefreshLookTowardsInput()
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::RefreshLookTowardsInput()"),
	                            STAT_UAlsAnimationInstance_RefreshLookTowardsInput, STATGROUP_Als)

	auto& LookTowardsInput{ViewState.LookTowardsInput};

	LookTowardsInput.bReinitializationRequired |= bPendingUpdate;

	const auto CharacterYawAngle{UE_REAL_TO_FLOAT(LocomotionState.Rotation.Yaw)};

	if (RotationMode != AlsRotationModeTags::VelocityDirection)
	{
		LookTowardsInput.WorldYawAngle = FRotator3f::NormalizeAxis(CharacterYawAngle + LookTowardsInput.YawAngle);
		LookTowardsInput.bReinitializationRequired = false;
		return;
	}

	if (MovementBase.bHasRelativeRotation)
	{
		// Offset the angle to keep it relative to the movement base.

		LookTowardsInput.WorldYawAngle = FRotator3f::NormalizeAxis(LookTowardsInput.WorldYawAngle + MovementBase.DeltaRotation.Yaw);
	}

	// Block look towards the input direction when the character is in the air to prevent head rotation "snap" when changing look sides.

	const auto TargetYawAngle{
		FRotator3f::NormalizeAxis((LocomotionState.bHasInput && LocomotionMode != AlsLocomotionModeTags::InAir
			                           ? LocomotionState.InputYawAngle
			                           : LocomotionState.TargetYawAngle) - CharacterYawAngle)
	};

	if (LookTowardsInput.bReinitializationRequired || Settings->View.LookTowardsInputYawAngleInterpolationSpeed <= 0.0f)
	{
		LookTowardsInput.YawAngle = TargetYawAngle;
	}
	else
	{
		const auto YawAngle{FRotator3f::NormalizeAxis(LookTowardsInput.WorldYawAngle - CharacterYawAngle)};
		auto DeltaYawAngle{FRotator3f::NormalizeAxis(TargetYawAngle - YawAngle)};

		if (DeltaYawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold)
		{
			DeltaYawAngle -= 360.0f;
		}
		else if (FMath::Abs(LocomotionState.YawSpeed) > UE_SMALL_NUMBER && FMath::Abs(TargetYawAngle) > 90.0f)
		{
			// When interpolating yaw angle, favor the character rotation direction, over the shortest rotation
			// direction, so that the rotation of the head remains synchronized with the rotation of the body.

			DeltaYawAngle = LocomotionState.YawSpeed > 0.0f ? FMath::Abs(DeltaYawAngle) : -FMath::Abs(DeltaYawAngle);
		}

		const auto InterpolationAmount{
			UAlsMath::ExponentialDecay(GetDeltaSeconds(), Settings->View.LookTowardsInputYawAngleInterpolationSpeed)
		};

		LookTowardsInput.YawAngle = FRotator3f::NormalizeAxis(YawAngle + DeltaYawAngle * InterpolationAmount);
	}

	LookTowardsInput.WorldYawAngle = FRotator3f::NormalizeAxis(CharacterYawAngle + LookTowardsInput.YawAngle);

	LookTowardsInput.YawAmount = LookTowardsInput.YawAngle / 360.0f + 0.5f;

	LookTowardsInput.bReinitializationRequired = false;
}

void UAlsAnimationInstance::ReinitializeLookTowardsCamera()
{
	ViewState.LookTowardsCamera.bReinitializationRequired = true;
}

void UAlsAnimationInstance::RefreshLookTowardsCamera()
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::RefreshLookTowardsCamera()"),
	                            STAT_UAlsAnimationInstance_RefreshLookTowardsCamera, STATGROUP_Als)

	auto& LookTowardsCamera{ViewState.LookTowardsCamera};

	LookTowardsCamera.bReinitializationRequired |= bPendingUpdate;

	const auto CharacterYawAngle{UE_REAL_TO_FLOAT(LocomotionState.Rotation.Yaw)};

	if (RotationMode == AlsRotationModeTags::VelocityDirection && LocomotionState.bMoving)
	{
		LookTowardsCamera.WorldYawAngle = FRotator3f::NormalizeAxis(CharacterYawAngle + LookTowardsCamera.YawAngle);
		LookTowardsCamera.bReinitializationRequired = false;
		return;
	}

	if (MovementBase.bHasRelativeRotation)
	{
		// Offset the angle to keep it relative to the movement base.

		LookTowardsCamera.WorldYawAngle = FRotator3f::NormalizeAxis(LookTowardsCamera.WorldYawAngle + MovementBase.DeltaRotation.Yaw);
	}

	const auto TargetYawAngle{ViewState.YawAngle};

	if (LookTowardsCamera.bReinitializationRequired || Settings->View.LookTowardsCameraRotationInterpolationSpeed <= 0.0f)
	{
		LookTowardsCamera.YawAngle = TargetYawAngle;
		LookTowardsCamera.PitchAngle = ViewState.PitchAngle;
	}
	else
	{
		const auto YawAngle{FRotator3f::NormalizeAxis(LookTowardsCamera.WorldYawAngle - CharacterYawAngle)};
		auto DeltaYawAngle{FRotator3f::NormalizeAxis(TargetYawAngle - YawAngle)};

		if (DeltaYawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold)
		{
			DeltaYawAngle -= 360.0f;
		}
		else if (FMath::Abs(LocomotionState.YawSpeed) > UE_SMALL_NUMBER && FMath::Abs(TargetYawAngle) > 90.0f)
		{
			// When interpolating yaw angle, favor the character rotation direction, over the shortest rotation
			// direction, so that the rotation of the head remains synchronized with the rotation of the body.

			DeltaYawAngle = LocomotionState.YawSpeed > 0.0f ? FMath::Abs(DeltaYawAngle) : -FMath::Abs(DeltaYawAngle);
		}

		const auto InterpolationAmount{
			UAlsMath::ExponentialDecay(GetDeltaSeconds(), Settings->View.LookTowardsCameraRotationInterpolationSpeed)
		};

		LookTowardsCamera.YawAngle = FRotator3f::NormalizeAxis(YawAngle + DeltaYawAngle * InterpolationAmount);
		LookTowardsCamera.PitchAngle = UAlsMath::LerpAngle(LookTowardsCamera.PitchAngle, ViewState.PitchAngle, InterpolationAmount);
	}

	LookTowardsCamera.WorldYawAngle = FRotator3f::NormalizeAxis(CharacterYawAngle + LookTowardsCamera.YawAngle);

	// Separate the yaw angle into 3 separate values. These 3 values are used to improve the
	// blending of the view when rotating completely around the character. This allows to
	// keep the view responsive but still smoothly blend from left to right or right to left.

	LookTowardsCamera.YawForwardAmount = LookTowardsCamera.YawAngle / 360.0f + 0.5f;
	LookTowardsCamera.YawLeftAmount = 0.5f - FMath::Abs(LookTowardsCamera.YawForwardAmount - 0.5f);
	LookTowardsCamera.YawRightAmount = 0.5f + FMath::Abs(LookTowardsCamera.YawForwardAmount - 0.5f);

	LookTowardsCamera.bReinitializationRequired = false;
}

void UAlsAnimationInstance::RefreshLocomotionOnGameThread()
{
	check(IsInGameThread())

	const auto& Locomotion{Character->GetLocomotionState()};

	LocomotionState.bHasInput = Locomotion.bHasInput;
	LocomotionState.InputYawAngle = Locomotion.InputYawAngle;

	LocomotionState.Speed = Locomotion.Speed;
	LocomotionState.Velocity = Locomotion.Velocity;
	LocomotionState.VelocityYawAngle = Locomotion.VelocityYawAngle;
	LocomotionState.Acceleration = Locomotion.Acceleration;

	const auto* Movement{Character->GetCharacterMovement()};

	LocomotionState.MaxAcceleration = Movement->GetMaxAcceleration();
	LocomotionState.MaxBrakingDeceleration = Movement->GetMaxBrakingDeceleration();
	LocomotionState.WalkableFloorZ = Movement->GetWalkableFloorZ();

	LocomotionState.bMoving = Locomotion.bMoving;

	LocomotionState.bMovingSmooth = (Locomotion.bHasInput && Locomotion.bHasSpeed) ||
	                                Locomotion.Speed > Settings->General.MovingSmoothSpeedThreshold;

	LocomotionState.TargetYawAngle = Locomotion.TargetYawAngle;
	LocomotionState.Location = Locomotion.Location;
	LocomotionState.Rotation = Locomotion.Rotation;
	LocomotionState.RotationQuaternion = Locomotion.RotationQuaternion;
	LocomotionState.YawSpeed = Locomotion.YawSpeed;

	LocomotionState.Scale = UE_REAL_TO_FLOAT(GetSkelMeshComponent()->GetComponentScale().Z);

	const auto* Capsule{Character->GetCapsuleComponent()};

	LocomotionState.CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	LocomotionState.CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
}

void UAlsAnimationInstance::RefreshGroundedOnGameThread()
{
	check(IsInGameThread())

	GroundedState.bPivotActive = GroundedState.bPivotActivationRequested && !bPendingUpdate &&
	                             LocomotionState.Speed < Settings->Grounded.PivotActivationSpeedThreshold;

	GroundedState.bPivotActivationRequested = false;
}

void UAlsAnimationInstance::RefreshGrounded(const float DeltaTime)
{
	// Always sample sprint block curve, otherwise issues with inertial blending may occur.

	GroundedState.SprintBlockAmount = GetCurveValueClamped01(UAlsConstants::SprintBlockCurveName());
	GroundedState.HipsDirectionLockAmount = FMath::Clamp(GetCurveValue(UAlsConstants::HipsDirectionLockCurveName()), -1.0f, 1.0f);

	if (LocomotionMode != AlsLocomotionModeTags::Grounded)
	{
		GroundedState.VelocityBlend.bReinitializationRequired = true;
		GroundedState.SprintTime = 0.0f;
		return;
	}

	if (!LocomotionState.bMoving)
	{
		ResetGroundedLeanAmount(DeltaTime);
		return;
	}

	// Calculate the relative acceleration amount. This value represents the current amount of acceleration / deceleration
	// relative to the character rotation. It is normalized to a range of -1 to 1 so that -1 equals the
	// max braking deceleration and 1 equals the max acceleration of the character movement component.

	FVector3f RelativeAccelerationAmount;

	if ((LocomotionState.Acceleration | LocomotionState.Velocity) >= 0.0f)
	{
		RelativeAccelerationAmount = UAlsMath::ClampMagnitude01(
			FVector3f{LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Acceleration)} /
			LocomotionState.MaxAcceleration);
	}
	else
	{
		RelativeAccelerationAmount = UAlsMath::ClampMagnitude01(
			FVector3f{LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Acceleration)} /
			LocomotionState.MaxBrakingDeceleration);
	}

	RefreshMovementDirection();
	RefreshVelocityBlend(DeltaTime);
	RefreshRotationYawOffsets();

	RefreshSprint(RelativeAccelerationAmount, DeltaTime);

	RefreshStrideBlendAmount();
	RefreshWalkRunBlendAmount();

	RefreshStandingPlayRate();
	RefreshCrouchingPlayRate();

	RefreshGroundedLeanAmount(RelativeAccelerationAmount, DeltaTime);
}

void UAlsAnimationInstance::RefreshMovementDirection()
{
	// Calculate the movement direction. This value represents the direction the character is moving relative
	// to the camera and is used in the cycle blending to blend to the appropriate directional states.

	if (Gait == AlsGaitTags::Sprinting)
	{
		GroundedState.MovementDirection = EAlsMovementDirection::Forward;
		return;
	}

	static constexpr auto ForwardHalfAngle{70.0f};

	GroundedState.MovementDirection = UAlsMath::CalculateMovementDirection(
		FRotator3f::NormalizeAxis(LocomotionState.VelocityYawAngle - UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw)),
		ForwardHalfAngle, 5.0f);
}

void UAlsAnimationInstance::RefreshVelocityBlend(const float DeltaTime)
{
	GroundedState.VelocityBlend.bReinitializationRequired |= bPendingUpdate;

	// Calculate and interpolate the velocity blend amounts. This value represents the velocity amount of
	// the character in each direction (normalized so that diagonals equal 0.5 for each direction) and is
	// used in a blend multi node to produce better directional blending than a standard blend space.

	const auto RelativeVelocityDirection{
		FVector3f{LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Velocity)}.GetSafeNormal()
	};

	const auto RelativeDirection{
		RelativeVelocityDirection /
		(FMath::Abs(RelativeVelocityDirection.X) + FMath::Abs(RelativeVelocityDirection.Y) + FMath::Abs(RelativeVelocityDirection.Z))
	};

	if (GroundedState.VelocityBlend.bReinitializationRequired)
	{
		GroundedState.VelocityBlend.bReinitializationRequired = false;

		GroundedState.VelocityBlend.ForwardAmount = UAlsMath::Clamp01(RelativeDirection.X);
		GroundedState.VelocityBlend.BackwardAmount = FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f));
		GroundedState.VelocityBlend.LeftAmount = FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f));
		GroundedState.VelocityBlend.RightAmount = UAlsMath::Clamp01(RelativeDirection.Y);
	}
	else
	{
		GroundedState.VelocityBlend.ForwardAmount = FMath::FInterpTo(GroundedState.VelocityBlend.ForwardAmount,
		                                                             UAlsMath::Clamp01(RelativeDirection.X), DeltaTime,
		                                                             Settings->Grounded.VelocityBlendInterpolationSpeed);

		GroundedState.VelocityBlend.BackwardAmount = FMath::FInterpTo(GroundedState.VelocityBlend.BackwardAmount,
		                                                              FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f)), DeltaTime,
		                                                              Settings->Grounded.VelocityBlendInterpolationSpeed);

		GroundedState.VelocityBlend.LeftAmount = FMath::FInterpTo(GroundedState.VelocityBlend.LeftAmount,
		                                                          FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f)), DeltaTime,
		                                                          Settings->Grounded.VelocityBlendInterpolationSpeed);

		GroundedState.VelocityBlend.RightAmount = FMath::FInterpTo(GroundedState.VelocityBlend.RightAmount,
		                                                           UAlsMath::Clamp01(RelativeDirection.Y), DeltaTime,
		                                                           Settings->Grounded.VelocityBlendInterpolationSpeed);
	}
}

void UAlsAnimationInstance::RefreshRotationYawOffsets()
{
	// Set the rotation yaw offsets. These values influence the rotation yaw offset curve in the
	// animation graph and are used to offset the character's rotation for more natural movement.
	// The curves allow for fine control over how the offset behaves for each movement direction.

	const auto RotationYawOffset{FRotator3f::NormalizeAxis(LocomotionState.VelocityYawAngle - UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw))};

	GroundedState.RotationYawOffsets.ForwardAngle = Settings->Grounded.RotationYawOffsetForwardCurve->GetFloatValue(RotationYawOffset);
	GroundedState.RotationYawOffsets.BackwardAngle = Settings->Grounded.RotationYawOffsetBackwardCurve->GetFloatValue(RotationYawOffset);
	GroundedState.RotationYawOffsets.LeftAngle = Settings->Grounded.RotationYawOffsetLeftCurve->GetFloatValue(RotationYawOffset);
	GroundedState.RotationYawOffsets.RightAngle = Settings->Grounded.RotationYawOffsetRightCurve->GetFloatValue(RotationYawOffset);
}

void UAlsAnimationInstance::RefreshSprint(const FVector3f& RelativeAccelerationAmount, const float DeltaTime)
{
	if (Gait != AlsGaitTags::Sprinting)
	{
		GroundedState.SprintTime = 0.0f;
		GroundedState.SprintAccelerationAmount = 0.0f;
		return;
	}

	// Use the relative acceleration as the sprint relative acceleration if less than 0.5 seconds has
	// elapsed since the start of the sprint, otherwise set the sprint relative acceleration to zero.
	// This is necessary to apply the acceleration animation only at the beginning of the sprint.

	static constexpr auto TimeThreshold{0.5f};

	GroundedState.SprintTime = bPendingUpdate
		                           ? TimeThreshold
		                           : GroundedState.SprintTime + DeltaTime;

	GroundedState.SprintAccelerationAmount = GroundedState.SprintTime >= TimeThreshold
		                                         ? 0.0f
		                                         : RelativeAccelerationAmount.X;
}

void UAlsAnimationInstance::RefreshStrideBlendAmount()
{
	// Calculate the stride blend amount. This value is used within the blend spaces to scale the stride (distance feet travel)
	// so that the character can walk or run at different movement speeds. It also allows the walk or run gait animations to
	// blend independently while still matching the animation speed to the movement speed, preventing the character from needing
	// to play a half walk + half run blend. The curves are used to map the stride amount to the speed for maximum control.

	const auto Speed{LocomotionState.Speed / LocomotionState.Scale};

	const auto StandingStrideBlend{
		FMath::Lerp(Settings->Grounded.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
		            Settings->Grounded.StrideBlendAmountRunCurve->GetFloatValue(Speed),
		            PoseState.UnweightedGaitRunningAmount)
	};

	// Crouching stride blend amount.

	GroundedState.StrideBlendAmount = FMath::Lerp(StandingStrideBlend,
	                                              Settings->Grounded.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
	                                              PoseState.CrouchingAmount);
}

void UAlsAnimationInstance::RefreshWalkRunBlendAmount()
{
	// Calculate the walk run blend amount. This value is used within the blend spaces to blend between walking and running.

	GroundedState.WalkRunBlendAmount = Gait == AlsGaitTags::Walking ? 0.0f : 1.0f;
}

void UAlsAnimationInstance::RefreshStandingPlayRate()
{
	// Calculate the standing play rate by dividing the character's speed by the animated speed for each gait.
	// The interpolation is determined by the gait amount curve that exists on every locomotion cycle so that
	// the play rate is always in sync with the currently blended animation. The value is also divided by the
	// stride blend and the capsule scale so that the play rate increases as the stride or scale gets smaller.

	const auto WalkRunSpeedAmount{
		FMath::Lerp(LocomotionState.Speed / Settings->Grounded.AnimatedWalkSpeed,
		            LocomotionState.Speed / Settings->Grounded.AnimatedRunSpeed,
		            PoseState.UnweightedGaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount{
		FMath::Lerp(WalkRunSpeedAmount,
		            LocomotionState.Speed / Settings->Grounded.AnimatedSprintSpeed,
		            PoseState.UnweightedGaitSprintingAmount)
	};

	GroundedState.StandingPlayRate = FMath::Clamp(
		WalkRunSprintSpeedAmount / (GroundedState.StrideBlendAmount * LocomotionState.Scale), 0.0f, 3.0f);
}

void UAlsAnimationInstance::RefreshCrouchingPlayRate()
{
	// Calculate the crouching play rate by dividing the character's speed by the animated speed. This value needs
	// to be separate from the standing play rate to improve the blend from crouching to standing while in motion.

	GroundedState.CrouchingPlayRate = FMath::Clamp(
		LocomotionState.Speed / (Settings->Grounded.AnimatedCrouchSpeed * GroundedState.StrideBlendAmount * LocomotionState.Scale),
		0.0f, 2.0f);
}

void UAlsAnimationInstance::RefreshGroundedLeanAmount(const FVector3f& RelativeAccelerationAmount, const float DeltaTime)
{
	if (bPendingUpdate)
	{
		LeanState.RightAmount = RelativeAccelerationAmount.Y;
		LeanState.ForwardAmount = RelativeAccelerationAmount.X;
	}
	else
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, RelativeAccelerationAmount.Y,
		                                         DeltaTime, Settings->General.LeanInterpolationSpeed);

		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, RelativeAccelerationAmount.X,
		                                           DeltaTime, Settings->General.LeanInterpolationSpeed);
	}
}

void UAlsAnimationInstance::ResetGroundedLeanAmount(const float DeltaTime)
{
	if (bPendingUpdate)
	{
		LeanState.RightAmount = 0.0f;
		LeanState.ForwardAmount = 0.0f;
	}
	else
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, 0.0f, DeltaTime, Settings->General.LeanInterpolationSpeed);
		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, 0.0f, DeltaTime, Settings->General.LeanInterpolationSpeed);
	}
}

void UAlsAnimationInstance::RefreshInAirOnGameThread()
{
	check(IsInGameThread())

	InAirState.bJumped = !bPendingUpdate && (InAirState.bJumped || InAirState.bJumpRequested);
	InAirState.bJumpRequested = false;
}

void UAlsAnimationInstance::RefreshInAir(const float DeltaTime)
{
	if (InAirState.bJumped)
	{
		static constexpr auto ReferenceSpeed{600.0f};
		static constexpr auto MinPlayRate{1.2f};
		static constexpr auto MaxPlayRate{1.5f};

		InAirState.JumpPlayRate = UAlsMath::LerpClamped(MinPlayRate, MaxPlayRate, LocomotionState.Speed / ReferenceSpeed);
	}

	if (LocomotionMode != AlsLocomotionModeTags::InAir)
	{
		return;
	}

	// A separate variable for vertical speed is used to determine at what speed the character landed on the ground.

	InAirState.VerticalVelocity = UE_REAL_TO_FLOAT(LocomotionState.Velocity.Z);

	RefreshGroundPredictionAmount();

	RefreshInAirLeanAmount(DeltaTime);
}

void UAlsAnimationInstance::RefreshGroundPredictionAmount()
{
	// Calculate the ground prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward and getting the "time" (range from 0 to 1, 1 being maximum, 0 being about to ground) till impact.
	// The ground prediction amount curve is used to control how the time affects the final amount for a smooth blend.

	static constexpr auto VerticalVelocityThreshold{-200.0f};

	if (InAirState.VerticalVelocity > VerticalVelocityThreshold)
	{
		InAirState.GroundPredictionAmount = 0.0f;
		return;
	}

	const auto AllowanceAmount{1.0f - GetCurveValueClamped01(UAlsConstants::GroundPredictionBlockCurveName())};
	if (AllowanceAmount <= UE_KINDA_SMALL_NUMBER)
	{
		InAirState.GroundPredictionAmount = 0.0f;
		return;
	}

	const auto SweepStartLocation{LocomotionState.Location};

	static constexpr auto MinVerticalVelocity{-4000.0f};
	static constexpr auto MaxVerticalVelocity{-200.0f};

	auto VelocityDirection{LocomotionState.Velocity};
	VelocityDirection.Z = FMath::Clamp(VelocityDirection.Z, MinVerticalVelocity, MaxVerticalVelocity);
	VelocityDirection.Normalize();

	static constexpr auto MinSweepDistance{150.0f};
	static constexpr auto MaxSweepDistance{2000.0f};

	const auto SweepVector{
		VelocityDirection * FMath::GetMappedRangeValueClamped(FVector2f{MaxVerticalVelocity, MinVerticalVelocity},
		                                                      {MinSweepDistance, MaxSweepDistance},
		                                                      InAirState.VerticalVelocity) * LocomotionState.Scale
	};

	FHitResult Hit;
	GetWorld()->SweepSingleByChannel(Hit, SweepStartLocation, SweepStartLocation + SweepVector, FQuat::Identity, ECC_WorldStatic,
	                                 FCollisionShape::MakeCapsule(LocomotionState.CapsuleRadius, LocomotionState.CapsuleHalfHeight),
	                                 {__FUNCTION__, false, Character}, Settings->InAir.GroundPredictionSweepResponses);

	const auto bGroundValid{Hit.IsValidBlockingHit() && Hit.ImpactNormal.Z >= LocomotionState.WalkableFloorZ};

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (bDisplayDebugTraces)
	{
		if (IsInGameThread())
		{
			UAlsUtility::DrawDebugSweepSingleCapsule(GetWorld(), Hit.TraceStart, Hit.TraceEnd, FRotator::ZeroRotator,
			                                         LocomotionState.CapsuleRadius, LocomotionState.CapsuleHalfHeight,
			                                         bGroundValid, Hit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f});
		}
		else
		{
			DisplayDebugTracesQueue.Add([this, Hit, bGroundValid]
				{
					UAlsUtility::DrawDebugSweepSingleCapsule(GetWorld(), Hit.TraceStart, Hit.TraceEnd, FRotator::ZeroRotator,
					                                         LocomotionState.CapsuleRadius, LocomotionState.CapsuleHalfHeight,
					                                         bGroundValid, Hit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f});
				}
			);
		}
	}
#endif

	InAirState.GroundPredictionAmount = bGroundValid
		                                    ? Settings->InAir.GroundPredictionAmountCurve->GetFloatValue(Hit.Time) * AllowanceAmount
		                                    : 0.0f;
}

void UAlsAnimationInstance::RefreshInAirLeanAmount(const float DeltaTime)
{
	// Use the relative velocity direction and amount to determine how much the character should lean
	// while in air. The lean amount curve gets the vertical velocity and is used as a multiplier to
	// smoothly reverse the leaning direction when transitioning from moving upwards to moving downwards.

	static constexpr auto ReferenceSpeed{350.0f};

	const auto RelativeVelocity{
		FVector3f{LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Velocity)} /
		ReferenceSpeed * Settings->InAir.LeanAmountCurve->GetFloatValue(InAirState.VerticalVelocity)
	};

	if (bPendingUpdate)
	{
		LeanState.RightAmount = RelativeVelocity.Y;
		LeanState.ForwardAmount = RelativeVelocity.X;
	}
	else
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, RelativeVelocity.Y,
		                                         DeltaTime, Settings->General.LeanInterpolationSpeed);

		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, RelativeVelocity.X,
		                                           DeltaTime, Settings->General.LeanInterpolationSpeed);
	}
}

void UAlsAnimationInstance::RefreshFeetOnGameThread()
{
	check(IsInGameThread())

	const auto* Mesh{GetSkelMeshComponent()};

	const auto FootLeftTargetTransform{
		Mesh->GetSocketTransform(Settings->General.bUseFootIkBones
			                         ? UAlsConstants::FootLeftIkBoneName()
			                         : UAlsConstants::FootLeftVirtualBoneName())
	};

	FeetState.Left.TargetLocation = FootLeftTargetTransform.GetLocation();
	FeetState.Left.TargetRotation = FootLeftTargetTransform.GetRotation();

	const auto FootRightTargetTransform{
		Mesh->GetSocketTransform(Settings->General.bUseFootIkBones
			                         ? UAlsConstants::FootRightIkBoneName()
			                         : UAlsConstants::FootRightVirtualBoneName())
	};

	FeetState.Right.TargetLocation = FootRightTargetTransform.GetLocation();
	FeetState.Right.TargetRotation = FootRightTargetTransform.GetRotation();
}

void UAlsAnimationInstance::RefreshFeet(const float DeltaTime)
{
	FeetState.FootPlantedAmount = FMath::Clamp(GetCurveValue(UAlsConstants::FootPlantedCurveName()), -1.0f, 1.0f);
	FeetState.FeetCrossingAmount = GetCurveValueClamped01(UAlsConstants::FeetCrossingCurveName());

	FeetState.MinMaxPelvisOffsetZ = FVector2D::ZeroVector;

	const auto ComponentTransformInverse{GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().Inverse()};

	RefreshFoot(FeetState.Left, UAlsConstants::FootLeftIkCurveName(),
	            UAlsConstants::FootLeftLockCurveName(), ComponentTransformInverse, DeltaTime);

	RefreshFoot(FeetState.Right, UAlsConstants::FootRightIkCurveName(),
	            UAlsConstants::FootRightLockCurveName(), ComponentTransformInverse, DeltaTime);

	FeetState.MinMaxPelvisOffsetZ.X = FMath::Min(FeetState.Left.OffsetTargetLocation.Z, FeetState.Right.OffsetTargetLocation.Z) /
	                                  LocomotionState.Scale;

	FeetState.MinMaxPelvisOffsetZ.Y = FMath::Max(FeetState.Left.OffsetTargetLocation.Z, FeetState.Right.OffsetTargetLocation.Z) /
	                                  LocomotionState.Scale;
}

void UAlsAnimationInstance::RefreshFoot(FAlsFootState& FootState, const FName& FootIkCurveName, const FName& FootLockCurveName,
                                        const FTransform& ComponentTransformInverse, const float DeltaTime) const
{
	FootState.IkAmount = GetCurveValueClamped01(FootIkCurveName);

	ProcessFootLockTeleport(FootState);

	ProcessFootLockBaseChange(FootState, ComponentTransformInverse);

	auto FinalLocation{FootState.TargetLocation};
	auto FinalRotation{FootState.TargetRotation};

	RefreshFootLock(FootState, FootLockCurveName, ComponentTransformInverse, DeltaTime, FinalLocation, FinalRotation);

	RefreshFootOffset(FootState, DeltaTime, FinalLocation, FinalRotation);

	FootState.IkLocation = ComponentTransformInverse.TransformPosition(FinalLocation);
	FootState.IkRotation = ComponentTransformInverse.TransformRotation(FinalRotation);
}

void UAlsAnimationInstance::ProcessFootLockTeleport(FAlsFootState& FootState) const
{
	// Due to network smoothing, we assume that teleportation occurs over a short period of time, not
	// in one frame, since after accepting the teleportation event, the character can still be moved for
	// some indefinite time, and this must be taken into account in order to avoid foot locking glitches.

	if (bPendingUpdate || GetWorld()->TimeSince(TeleportedTime) > 0.2f ||
	    !FAnimWeight::IsRelevant(FootState.IkAmount * FootState.LockAmount))
	{
		return;
	}

	const auto& ComponentTransform{GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform()};

	FootState.LockLocation = ComponentTransform.TransformPosition(FootState.LockComponentRelativeLocation);
	FootState.LockRotation = ComponentTransform.TransformRotation(FootState.LockComponentRelativeRotation);

	if (MovementBase.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{MovementBase.Rotation.Inverse()};

		FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location);
		FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FootState.LockRotation;
	}
}

void UAlsAnimationInstance::ProcessFootLockBaseChange(FAlsFootState& FootState, const FTransform& ComponentTransformInverse) const
{
	if ((!bPendingUpdate && !MovementBase.bBaseChanged) || !FAnimWeight::IsRelevant(FootState.IkAmount * FootState.LockAmount))
	{
		return;
	}

	if (bPendingUpdate)
	{
		FootState.LockLocation = FootState.TargetLocation;
		FootState.LockRotation = FootState.TargetRotation;
	}

	FootState.LockComponentRelativeLocation = ComponentTransformInverse.TransformPosition(FootState.LockLocation);
	FootState.LockComponentRelativeRotation = ComponentTransformInverse.TransformRotation(FootState.LockRotation);

	if (MovementBase.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{MovementBase.Rotation.Inverse()};

		FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.RotateVector(FootState.LockLocation - MovementBase.Location);
		FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FootState.LockRotation;
	}
	else
	{
		FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
		FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
	}
}

void UAlsAnimationInstance::RefreshFootLock(FAlsFootState& FootState, const FName& FootLockCurveName,
                                            const FTransform& ComponentTransformInverse, const float DeltaTime,
                                            FVector& FinalLocation, FQuat& FinalRotation) const
{
	auto NewFootLockAmount{GetCurveValueClamped01(FootLockCurveName)};

	NewFootLockAmount *= 1.0f - RotateInPlaceState.FootLockBlockAmount;

	if (LocomotionState.bMovingSmooth || LocomotionMode != AlsLocomotionModeTags::Grounded)
	{
		// Smoothly disable foot locking if the character is moving or in the air,
		// instead of relying on the curve value from the animation blueprint.

		static constexpr auto MovingDecreaseSpeed{5.0f};
		static constexpr auto NotGroundedDecreaseSpeed{0.6f};

		NewFootLockAmount = bPendingUpdate
			                    ? 0.0f
			                    : FMath::Max(0.0f, FMath::Min(NewFootLockAmount,
			                                                  FootState.LockAmount - DeltaTime *
			                                                  (LocomotionState.bMovingSmooth
				                                                   ? MovingDecreaseSpeed
				                                                   : NotGroundedDecreaseSpeed)));
	}

	if (Settings->Feet.bDisableFootLock || !FAnimWeight::IsRelevant(FootState.IkAmount * NewFootLockAmount))
	{
		if (FootState.LockAmount > 0.0f)
		{
			FootState.LockAmount = 0.0f;

			FootState.LockLocation = FVector::ZeroVector;
			FootState.LockRotation = FQuat::Identity;

			FootState.LockComponentRelativeLocation = FVector::ZeroVector;
			FootState.LockComponentRelativeRotation = FQuat::Identity;

			FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
			FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
		}

		return;
	}

	const auto bNewAmountEqualOne{FAnimWeight::IsFullWeight(NewFootLockAmount)};
	const auto bNewAmountGreaterThanPrevious{NewFootLockAmount > FootState.LockAmount};

	// Update the foot lock amount only if the new amount is less than the current amount or equal to 1. This
	// allows the foot to blend out from a locked location or lock to a new location, but never blend in.

	if (bNewAmountEqualOne)
	{
		if (bNewAmountGreaterThanPrevious)
		{
			// If the new foot lock amount is 1 and the previous amount is less than 1, then save the new foot lock location and rotation.

			if (FootState.LockAmount <= 0.9f)
			{
				// Keep the same lock location and rotation when the previous lock
				// amount is close to 1 to get rid of the foot "teleportation" issue.

				FootState.LockLocation = FinalLocation;
				FootState.LockRotation = FinalRotation;
			}

			if (MovementBase.bHasRelativeLocation)
			{
				const auto BaseRotationInverse{MovementBase.Rotation.Inverse()};

				FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.RotateVector(FinalLocation - MovementBase.Location);
				FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FinalRotation;
			}
			else
			{
				FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
				FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
			}
		}

		FootState.LockAmount = 1.0f;
	}
	else if (!bNewAmountGreaterThanPrevious)
	{
		FootState.LockAmount = NewFootLockAmount;
	}

	if (MovementBase.bHasRelativeLocation)
	{
		FootState.LockLocation = MovementBase.Location + MovementBase.Rotation.RotateVector(FootState.LockMovementBaseRelativeLocation);
		FootState.LockRotation = MovementBase.Rotation * FootState.LockMovementBaseRelativeRotation;
	}

	FootState.LockComponentRelativeLocation = ComponentTransformInverse.TransformPosition(FootState.LockLocation);
	FootState.LockComponentRelativeRotation = ComponentTransformInverse.TransformRotation(FootState.LockRotation);

	FinalLocation = FMath::Lerp(FinalLocation, FootState.LockLocation, FootState.LockAmount);
	FinalRotation = FQuat::Slerp(FinalRotation, FootState.LockRotation, FootState.LockAmount);
}

void UAlsAnimationInstance::RefreshFootOffset(FAlsFootState& FootState, const float DeltaTime,
                                              FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FAnimWeight::IsRelevant(FootState.IkAmount))
	{
		FootState.OffsetTargetLocation = FVector::ZeroVector;
		FootState.OffsetTargetRotation = FQuat::Identity;
		FootState.OffsetSpringState.Reset();
		return;
	}

	if (LocomotionMode == AlsLocomotionModeTags::InAir)
	{
		FootState.OffsetTargetLocation = FVector::ZeroVector;
		FootState.OffsetTargetRotation = FQuat::Identity;
		FootState.OffsetSpringState.Reset();

		if (bPendingUpdate)
		{
			FootState.OffsetLocation = FVector::ZeroVector;
			FootState.OffsetRotation = FQuat::Identity;
		}
		else
		{
			static constexpr auto InterpolationSpeed{15.0f};

			FootState.OffsetLocation = FMath::VInterpTo(FootState.OffsetLocation, FVector::ZeroVector, DeltaTime, InterpolationSpeed);
			FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, FQuat::Identity, DeltaTime, InterpolationSpeed);

			FinalLocation += FootState.OffsetLocation;
			FinalRotation = FootState.OffsetRotation * FinalRotation;
		}

		return;
	}

	// Trace downward from the foot location to find the geometry. If the surface is walkable, save the impact location and normal.

	const FVector TraceLocation{
		FinalLocation.X, FinalLocation.Y, GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().GetLocation().Z
	};

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit,
	                                     TraceLocation + FVector{
		                                     0.0f, 0.0f, Settings->Feet.IkTraceDistanceUpward * LocomotionState.Scale
	                                     },
	                                     TraceLocation - FVector{
		                                     0.0f, 0.0f, Settings->Feet.IkTraceDistanceDownward * LocomotionState.Scale
	                                     },
	                                     UEngineTypes::ConvertToCollisionChannel(Settings->Feet.IkTraceChannel),
	                                     {__FUNCTION__, true, Character});

	const auto bGroundValid{Hit.IsValidBlockingHit() && Hit.ImpactNormal.Z >= LocomotionState.WalkableFloorZ};

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	if (bDisplayDebugTraces)
	{
		if (IsInGameThread())
		{
			UAlsUtility::DrawDebugLineTraceSingle(GetWorld(), Hit.TraceStart, Hit.TraceEnd, bGroundValid,
			                                      Hit, {0.0f, 0.25f, 1.0f}, {0.0f, 0.75f, 1.0f});
		}
		else
		{
			DisplayDebugTracesQueue.Add([this, Hit, bGroundValid]
				{
					UAlsUtility::DrawDebugLineTraceSingle(GetWorld(), Hit.TraceStart, Hit.TraceEnd, bGroundValid,
					                                      Hit, {0.0f, 0.25f, 1.0f}, {0.0f, 0.75f, 1.0f});
				}
			);
		}
	}
#endif

	if (bGroundValid)
	{
		const auto FootHeight{Settings->Feet.FootHeight * LocomotionState.Scale};

		// Find the difference in location between the impact location and the expected (flat) floor location. These
		// values are offset by the impact normal multiplied by the foot height to get better behavior on angled surfaces.

		FootState.OffsetTargetLocation = Hit.ImpactPoint - TraceLocation + Hit.ImpactNormal * FootHeight;
		FootState.OffsetTargetLocation.Z -= FootHeight;

		// Calculate the rotation offset.

		FootState.OffsetTargetRotation = FRotator{
			-UAlsMath::DirectionToAngle({Hit.ImpactNormal.Z, Hit.ImpactNormal.X}),
			0.0f,
			UAlsMath::DirectionToAngle({Hit.ImpactNormal.Z, Hit.ImpactNormal.Y})
		}.Quaternion();
	}

	// Interpolate current offsets to the new target values.

	if (bPendingUpdate)
	{
		FootState.OffsetSpringState.Reset();

		FootState.OffsetLocation = FootState.OffsetTargetLocation;
		FootState.OffsetRotation = FootState.OffsetTargetRotation;
	}
	else
	{
		static constexpr auto LocationInterpolationFrequency{0.4f};
		static constexpr auto LocationInterpolationDampingRatio{4.0f};
		static constexpr auto LocationInterpolationTargetVelocityAmount{1.0f};

		FootState.OffsetLocation = UAlsMath::SpringDampVector(FootState.OffsetLocation, FootState.OffsetTargetLocation,
		                                                      FootState.OffsetSpringState, DeltaTime, LocationInterpolationFrequency,
		                                                      LocationInterpolationDampingRatio, LocationInterpolationTargetVelocityAmount);

		static constexpr auto RotationInterpolationSpeed{30.0f};

		FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, FootState.OffsetTargetRotation,
		                                            DeltaTime, RotationInterpolationSpeed);
	}

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;
}

void UAlsAnimationInstance::PlayQuickStopAnimation()
{
	if (RotationMode != AlsRotationModeTags::VelocityDirection)
	{
		PlayTransitionLeftAnimation(Settings->Transitions.QuickStopBlendInDuration, Settings->Transitions.QuickStopBlendOutDuration,
		                            Settings->Transitions.QuickStopPlayRate.X, Settings->Transitions.QuickStopStartTime);
		return;
	}

	auto RotationYawAngle{
		FRotator3f::NormalizeAxis((LocomotionState.bHasInput ? LocomotionState.InputYawAngle : LocomotionState.TargetYawAngle) -
		                          UE_REAL_TO_FLOAT(LocomotionState.Rotation.Yaw))
	};

	if (RotationYawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold)
	{
		RotationYawAngle -= 360.0f;
	}

	// Scale quick stop animation play rate based on how far the character
	// is going to rotate. At 180 degrees, the play rate will be maximal.

	if (RotationYawAngle <= 0.0f)
	{
		PlayTransitionLeftAnimation(Settings->Transitions.QuickStopBlendInDuration, Settings->Transitions.QuickStopBlendOutDuration,
		                            FMath::Lerp(Settings->Transitions.QuickStopPlayRate.X, Settings->Transitions.QuickStopPlayRate.Y,
		                                        FMath::Abs(RotationYawAngle) / 180.0f), Settings->Transitions.QuickStopStartTime);
	}
	else
	{
		PlayTransitionRightAnimation(Settings->Transitions.QuickStopBlendInDuration, Settings->Transitions.QuickStopBlendOutDuration,
		                             FMath::Lerp(Settings->Transitions.QuickStopPlayRate.X, Settings->Transitions.QuickStopPlayRate.Y,
		                                         FMath::Abs(RotationYawAngle) / 180.0f), Settings->Transitions.QuickStopStartTime);
	}
}

void UAlsAnimationInstance::PlayTransitionAnimation(UAnimSequenceBase* Animation, const float BlendInDuration, const float BlendOutDuration,
                                                    const float PlayRate, const float StartTime, const bool bFromStandingIdleOnly)
{
	check(IsInGameThread())

	if (!IsValid(Character))
	{
		return;
	}

	if (bFromStandingIdleOnly && (Character->GetLocomotionState().bMoving || Character->GetStance() != AlsStanceTags::Standing))
	{
		return;
	}

	PlaySlotAnimationAsDynamicMontage(Animation, UAlsConstants::TransitionSlotName(),
	                                  BlendInDuration, BlendOutDuration, PlayRate, 1, 0.0f, StartTime);
}

void UAlsAnimationInstance::PlayTransitionLeftAnimation(const float BlendInDuration, const float BlendOutDuration, const float PlayRate,
                                                        const float StartTime, const bool bFromStandingIdleOnly)
{
	if (!IsValid(Settings))
	{
		return;
	}

	PlayTransitionAnimation(Stance == AlsStanceTags::Crouching
		                        ? Settings->Transitions.CrouchingTransitionLeftAnimation
		                        : Settings->Transitions.StandingTransitionLeftAnimation,
	                        BlendInDuration, BlendOutDuration, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UAlsAnimationInstance::PlayTransitionRightAnimation(const float BlendInDuration, const float BlendOutDuration, const float PlayRate,
                                                         const float StartTime, const bool bFromStandingIdleOnly)
{
	if (!IsValid(Settings))
	{
		return;
	}

	PlayTransitionAnimation(Stance == AlsStanceTags::Crouching
		                        ? Settings->Transitions.CrouchingTransitionRightAnimation
		                        : Settings->Transitions.StandingTransitionRightAnimation,
	                        BlendInDuration, BlendOutDuration, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UAlsAnimationInstance::StopTransitionAndTurnInPlaceAnimations(const float BlendOutDuration)
{
	check(IsInGameThread())

	StopSlotAnimation(BlendOutDuration, UAlsConstants::TransitionSlotName());
	StopSlotAnimation(BlendOutDuration, UAlsConstants::TurnInPlaceStandingSlotName());
	StopSlotAnimation(BlendOutDuration, UAlsConstants::TurnInPlaceCrouchingSlotName());
}

void UAlsAnimationInstance::RefreshTransitions()
{
	// The allow transitions curve is modified within certain states, so that transitions allowed will be true while in those states.

	TransitionsState.bTransitionsAllowed = FAnimWeight::IsFullWeight(GetCurveValue(UAlsConstants::AllowTransitionsCurveName()));

	RefreshDynamicTransition();
}

void UAlsAnimationInstance::RefreshDynamicTransition()
{
	if (TransitionsState.DynamicTransitionsFrameDelay > 0)
	{
		TransitionsState.DynamicTransitionsFrameDelay -= 1;
		return;
	}

	if (!TransitionsState.bTransitionsAllowed || LocomotionState.bMoving || LocomotionMode != AlsLocomotionModeTags::Grounded)
	{
		return;
	}

	// Check each foot to see if the location difference between the foot look and its desired / target location
	// exceeds a threshold. If it does, play an additive transition animation on that foot. The currently set
	// transition plays the second half of a 2 foot transition animation, so that only a single foot moves.

	const auto FootLockDistanceThresholdSquared{
		FMath::Square(Settings->Transitions.DynamicTransitionFootLockDistanceThreshold * LocomotionState.Scale)
	};

	const auto FootLockLeftDistanceSquared{FVector::DistSquared(FeetState.Left.TargetLocation, FeetState.Left.LockLocation)};
	const auto FootLockRightDistanceSquared{FVector::DistSquared(FeetState.Right.TargetLocation, FeetState.Right.LockLocation)};

	const auto bTransitionLeftAllowed{
		FAnimWeight::IsRelevant(FeetState.Left.LockAmount) && FootLockLeftDistanceSquared > FootLockDistanceThresholdSquared
	};

	const auto bTransitionRightAllowed{
		FAnimWeight::IsRelevant(FeetState.Right.LockAmount) && FootLockRightDistanceSquared > FootLockDistanceThresholdSquared
	};

	if (!bTransitionLeftAllowed && !bTransitionRightAllowed)
	{
		return;
	}

	TObjectPtr<UAnimSequenceBase> DynamicTransitionAnimation;

	// If both transitions are allowed, choose the one with a greater lock distance.

	if (!bTransitionLeftAllowed)
	{
		DynamicTransitionAnimation = Stance == AlsStanceTags::Crouching
			                             ? Settings->Transitions.CrouchingDynamicTransitionRightAnimation
			                             : Settings->Transitions.StandingDynamicTransitionRightAnimation;
	}
	else if (!bTransitionRightAllowed)
	{
		DynamicTransitionAnimation = Stance == AlsStanceTags::Crouching
			                             ? Settings->Transitions.CrouchingDynamicTransitionLeftAnimation
			                             : Settings->Transitions.StandingDynamicTransitionLeftAnimation;
	}
	else if (FootLockLeftDistanceSquared >= FootLockRightDistanceSquared)
	{
		DynamicTransitionAnimation = Stance == AlsStanceTags::Crouching
			                             ? Settings->Transitions.CrouchingDynamicTransitionLeftAnimation
			                             : Settings->Transitions.StandingDynamicTransitionLeftAnimation;
	}
	else
	{
		DynamicTransitionAnimation = Stance == AlsStanceTags::Crouching
			                             ? Settings->Transitions.CrouchingDynamicTransitionRightAnimation
			                             : Settings->Transitions.StandingDynamicTransitionRightAnimation;
	}

	if (IsValid(DynamicTransitionAnimation))
	{
		// Block next dynamic transitions for about 2 frames to give the animation blueprint some time to properly react to the animation.

		TransitionsState.DynamicTransitionsFrameDelay = 2;

		// Animation montages can't be played in the worker thread, so queue them up to play later in the game thread.

		TransitionsState.QueuedDynamicTransitionAnimation = DynamicTransitionAnimation;

		if (IsInGameThread())
		{
			PlayQueuedDynamicTransitionAnimation();
		}
	}
}

void UAlsAnimationInstance::PlayQueuedDynamicTransitionAnimation()
{
	check(IsInGameThread())

	PlaySlotAnimationAsDynamicMontage(TransitionsState.QueuedDynamicTransitionAnimation, UAlsConstants::TransitionSlotName(),
	                                  Settings->Transitions.DynamicTransitionBlendDuration,
	                                  Settings->Transitions.DynamicTransitionBlendDuration,
	                                  Settings->Transitions.DynamicTransitionPlayRate, 1, 0.0f);

	TransitionsState.QueuedDynamicTransitionAnimation = nullptr;
}

bool UAlsAnimationInstance::IsRotateInPlaceAllowed()
{
	return RotationMode == AlsRotationModeTags::Aiming || ViewMode == AlsViewModeTags::FirstPerson;
}

void UAlsAnimationInstance::RefreshRotateInPlace(const float DeltaTime)
{
	static constexpr auto PlayRateInterpolationSpeed{5.0f};

	// Rotate in place is allowed only if the character is standing still and aiming or in first-person view mode.

	if (LocomotionState.bMoving || LocomotionMode != AlsLocomotionModeTags::Grounded || !IsRotateInPlaceAllowed())
	{
		RotateInPlaceState.bRotatingLeft = false;
		RotateInPlaceState.bRotatingRight = false;

		RotateInPlaceState.PlayRate = bPendingUpdate
			                              ? Settings->RotateInPlace.PlayRate.X
			                              : FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlace.PlayRate.X,
			                                                 DeltaTime, PlayRateInterpolationSpeed);

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// Check if the character should rotate left or right by checking if the view yaw angle exceeds the threshold.

	RotateInPlaceState.bRotatingLeft = ViewState.YawAngle < -Settings->RotateInPlace.ViewYawAngleThreshold;
	RotateInPlaceState.bRotatingRight = ViewState.YawAngle > Settings->RotateInPlace.ViewYawAngleThreshold;

	if (!RotateInPlaceState.bRotatingLeft && !RotateInPlaceState.bRotatingRight)
	{
		RotateInPlaceState.PlayRate = bPendingUpdate
			                              ? Settings->RotateInPlace.PlayRate.X
			                              : FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlace.PlayRate.X,
			                                                 DeltaTime, PlayRateInterpolationSpeed);

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// If the character should rotate, set the play rate to scale with the view yaw
	// speed. This makes the character rotate faster when moving the camera faster.

	const auto PlayRate{
		FMath::GetMappedRangeValueClamped(Settings->RotateInPlace.ReferenceViewYawSpeed,
		                                  Settings->RotateInPlace.PlayRate, ViewState.YawSpeed)
	};

	RotateInPlaceState.PlayRate = bPendingUpdate
		                              ? PlayRate
		                              : FMath::FInterpTo(RotateInPlaceState.PlayRate, PlayRate,
		                                                 DeltaTime, PlayRateInterpolationSpeed);

	// Disable foot locking when rotating at a large angle or rotating too fast, otherwise the legs may twist in a spiral.

	static constexpr auto BlockInterpolationSpeed{5.0f};

	RotateInPlaceState.FootLockBlockAmount =
		Settings->RotateInPlace.bDisableFootLock
			? 1.0f
			: FMath::Abs(ViewState.YawAngle) > Settings->RotateInPlace.FootLockBlockViewYawAngleThreshold
			? 0.5f
			: ViewState.YawSpeed <= Settings->RotateInPlace.FootLockBlockViewYawSpeedThreshold
			? 0.0f
			: bPendingUpdate
			? 1.0f
			: FMath::FInterpTo(RotateInPlaceState.FootLockBlockAmount, 1.0f, DeltaTime, BlockInterpolationSpeed);
}

bool UAlsAnimationInstance::IsTurnInPlaceAllowed()
{
	return RotationMode == AlsRotationModeTags::ViewDirection && ViewMode != AlsViewModeTags::FirstPerson;
}

void UAlsAnimationInstance::RefreshTurnInPlace(const float DeltaTime)
{
	// Turn in place is allowed only if transitions are allowed, the character
	// standing still and looking at the camera and not in first-person mode.

	if (LocomotionState.bMoving || LocomotionMode != AlsLocomotionModeTags::Grounded || !IsTurnInPlaceAllowed())
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		TurnInPlaceState.bFootLockDisabled = false;
		return;
	}

	if (!TransitionsState.bTransitionsAllowed)
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		return;
	}

	// Check if the view yaw speed is below the threshold and if the view yaw angle is outside the
	// threshold. If so, begin counting the activation delay time. If not, reset the activation delay
	// time. This ensures the conditions remain true for a sustained time before turning in place.

	if (ViewState.YawSpeed >= Settings->TurnInPlace.ViewYawSpeedThreshold ||
	    FMath::Abs(ViewState.YawAngle) <= Settings->TurnInPlace.ViewYawAngleThreshold)
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		TurnInPlaceState.bFootLockDisabled = false;
		return;
	}

	TurnInPlaceState.ActivationDelay = bPendingUpdate
		                                   ? 0.0f
		                                   : TurnInPlaceState.ActivationDelay + DeltaTime;

	const auto ActivationDelay{
		FMath::GetMappedRangeValueClamped({Settings->TurnInPlace.ViewYawAngleThreshold, 180.0f},
		                                  Settings->TurnInPlace.ViewYawAngleToActivationDelay,
		                                  FMath::Abs(ViewState.YawAngle))
	};

	// Check if the activation delay time exceeds the set delay (mapped to the view yaw angle). If so, start a turn in place.

	if (TurnInPlaceState.ActivationDelay <= ActivationDelay)
	{
		return;
	}

	// Select settings based on turn angle and stance.

	UAlsTurnInPlaceSettings* TurnInPlaceSettings{nullptr};
	FName TurnInPlaceSlotName;

	if (Stance == AlsStanceTags::Standing)
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceStandingSlotName();

		if (FMath::Abs(ViewState.YawAngle) < Settings->TurnInPlace.Turn180AngleThreshold)
		{
			TurnInPlaceSettings = ViewState.YawAngle <= 0.0f ||
			                      ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? Settings->TurnInPlace.StandingTurn90Left
				                      : Settings->TurnInPlace.StandingTurn90Right;
		}
		else
		{
			TurnInPlaceSettings = ViewState.YawAngle <= 0.0f ||
			                      ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? Settings->TurnInPlace.StandingTurn180Left
				                      : Settings->TurnInPlace.StandingTurn180Right;
		}
	}
	else if (Stance == AlsStanceTags::Crouching)
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceCrouchingSlotName();

		if (FMath::Abs(ViewState.YawAngle) < Settings->TurnInPlace.Turn180AngleThreshold)
		{
			TurnInPlaceSettings = ViewState.YawAngle <= 0.0f ||
			                      ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? Settings->TurnInPlace.CrouchingTurn90Left
				                      : Settings->TurnInPlace.CrouchingTurn90Right;
		}
		else
		{
			TurnInPlaceSettings = ViewState.YawAngle <= 0.0f ||
			                      ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? Settings->TurnInPlace.CrouchingTurn180Left
				                      : Settings->TurnInPlace.CrouchingTurn180Right;
		}
	}

	if (IsValid(TurnInPlaceSettings) && ALS_ENSURE(IsValid(TurnInPlaceSettings->Animation)))
	{
		// Animation montages can't be played in the worker thread, so queue them up to play later in the game thread.

		TurnInPlaceState.QueuedSettings = TurnInPlaceSettings;
		TurnInPlaceState.QueuedSlotName = TurnInPlaceSlotName;
		TurnInPlaceState.QueuedTurnYawAngle = ViewState.YawAngle;

		if (IsInGameThread())
		{
			PlayQueuedTurnInPlaceAnimation();
		}
	}
}

void UAlsAnimationInstance::PlayQueuedTurnInPlaceAnimation()
{
	check(IsInGameThread())

	if (!IsValid(TurnInPlaceState.QueuedSettings))
	{
		return;
	}

	const auto* TurnInPlaceSettings{TurnInPlaceState.QueuedSettings.Get()};

	PlaySlotAnimationAsDynamicMontage(TurnInPlaceSettings->Animation, TurnInPlaceState.QueuedSlotName,
	                                  Settings->TurnInPlace.BlendDuration, Settings->TurnInPlace.BlendDuration,
	                                  TurnInPlaceSettings->PlayRate, 1, 0.0f);

	// Scale the rotation yaw delta (gets scaled in animation graph) to compensate for play rate and turn angle (if allowed).

	TurnInPlaceState.PlayRate = TurnInPlaceSettings->bScalePlayRateByAnimatedTurnAngle
		                            ? TurnInPlaceSettings->PlayRate *
		                              FMath::Abs(TurnInPlaceState.QueuedTurnYawAngle / TurnInPlaceSettings->AnimatedTurnAngle)
		                            : TurnInPlaceSettings->PlayRate;

	TurnInPlaceState.bFootLockDisabled = Settings->TurnInPlace.bDisableFootLock;

	TurnInPlaceState.QueuedSettings = nullptr;
	TurnInPlaceState.QueuedSlotName = NAME_None;
	TurnInPlaceState.QueuedTurnYawAngle = 0.0f;
}

void UAlsAnimationInstance::RefreshRagdollingOnGameThread()
{
	check(IsInGameThread())

	if (LocomotionAction != AlsLocomotionActionTags::Ragdolling)
	{
		return;
	}

	// Scale the flail play rate by the root speed. The faster the ragdoll moves, the faster the character will flail.

	static constexpr auto ReferenceSpeed{1000.0f};

	RagdollingState.FlailPlayRate = UAlsMath::Clamp01(
		UE_REAL_TO_FLOAT(GetSkelMeshComponent()->GetPhysicsLinearVelocity(UAlsConstants::RootBoneName()).Size() / ReferenceSpeed));
}

void UAlsAnimationInstance::StopRagdolling()
{
	check(IsInGameThread())

	// Save a snapshot of the current ragdoll pose for use in animation graph to blend out of the ragdoll.

	SnapshotPose(RagdollingState.FinalRagdollPose);
}

float UAlsAnimationInstance::GetCurveValueClamped01(const FName& CurveName) const
{
	return UAlsMath::Clamp01(GetCurveValue(CurveName));
}
