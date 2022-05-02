#include "AlsAnimationInstance.h"

#include "AlsCharacter.h"
#include "TimerManager.h"
#include "Animation/AnimInstanceProxy.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsAnimationInstanceSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsLog.h"
#include "Utility/AlsUtility.h"
#include "Utility/GameplayTags/AlsLocomotionActionTags.h"
#include "Utility/GameplayTags/AlsLocomotionModeTags.h"

UAlsAnimationInstance::UAlsAnimationInstance()
{
	RootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;
}

void UAlsAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AAlsCharacter>(GetOwningActor());

	if (!GetWorld()->IsGameWorld() && Character.IsNull())
	{
		// Use default objects for editor preview.

		Character = GetMutableDefault<AAlsCharacter>();
	}
}

void UAlsAnimationInstance::NativeBeginPlay()
{
	check(!Settings.IsNull())
	check(!Character.IsNull())

	Character->GetCapsuleComponent()->TransformUpdated.AddWeakLambda(
		this, [&bTeleportedGameThread = bTeleportedGameThread](USceneComponent*, const EUpdateTransformFlags,
		                                                       const ETeleportType TeleportType)
		{
			bTeleportedGameThread |= TeleportType != ETeleportType::None;
		});

	Super::NativeBeginPlay();
}

void UAlsAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::NativeUpdateAnimation()"),
	                            STAT_UAlsAnimationInstance_NativeUpdateAnimation, STATGROUP_Als)

	Super::NativeUpdateAnimation(DeltaTime);

	if (Settings.IsNull() || Character.IsNull())
	{
		return;
	}

	Character->ApplyRotationYawSpeedFromAnimationInstance(DeltaTime);

	bPendingUpdate |= bPendingUpdateGameThread;
	bAnimationCurvesRelevant = bAnimationCurvesRelevantGameThread;
	bTeleported |= bTeleportedGameThread;

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	bDisplayDebugTraces = UAlsUtility::ShouldDisplayDebug(Character, UAlsConstants::TracesDisplayName());
#endif

	Stance = Character->GetStance();
	Gait = Character->GetGait();
	RotationMode = Character->GetRotationMode();
	LocomotionMode = Character->GetLocomotionMode();
	LocomotionAction = Character->GetLocomotionAction();
	ViewMode = Character->GetViewMode();
	OverlayMode = Character->GetOverlayMode();

	RefreshViewGameThread();

	RefreshLocomotionGameThread();
	RefreshGroundedGameThread();
	RefreshInAirGameThread();

	RefreshFeetGameThread();

	RefreshTransitionsGameThread();
	RefreshTurnInPlaceGameThread();

	RefreshRagdollingGameThread();

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

	bPendingUpdateGameThread = false;
	bTeleportedGameThread = false;
}

void UAlsAnimationInstance::NativeThreadSafeUpdateAnimation(const float DeltaTime)
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UAlsAnimationInstance::NativeThreadSafeUpdateAnimation()"),
	                            STAT_UAlsAnimationInstance_NativeThreadSafeUpdateAnimation, STATGROUP_Als)

	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (Settings.IsNull() || Character.IsNull())
	{
		return;
	}

	RefreshLayering();
	RefreshPose();

	RefreshView(DeltaTime);

	RefreshLocomotion(DeltaTime);
	RefreshGrounded(DeltaTime);
	RefreshInAir(DeltaTime);

	RefreshFeet(DeltaTime);

	RefreshTransitions();
	RefreshRotateInPlace(DeltaTime);
	RefreshTurnInPlace(DeltaTime);

	bPendingUpdate = false;
	bTeleported = false;
}

void UAlsAnimationInstance::RefreshLayering()
{
	if (!bAnimationCurvesRelevant)
	{
		return;
	}

	LayeringState.HeadBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerHeadCurve());
	LayeringState.HeadAdditiveBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerHeadAdditiveCurve());
	LayeringState.HeadSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerHeadSlotCurve());

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmLeftBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmLeftCurve());
	LayeringState.ArmLeftAdditiveBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmLeftAdditiveCurve());
	LayeringState.ArmLeftSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmLeftSlotCurve());
	LayeringState.ArmLeftLocalSpaceBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmLeftLocalSpaceCurve());
	LayeringState.ArmLeftMeshSpaceBlendAmount = !FAnimWeight::IsFullWeight(LayeringState.ArmLeftLocalSpaceBlendAmount);

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmRightBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightCurve());
	LayeringState.ArmRightAdditiveBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightAdditiveCurve());
	LayeringState.ArmRightSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightSlotCurve());
	LayeringState.ArmRightLocalSpaceBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightLocalSpaceCurve());
	LayeringState.ArmRightMeshSpaceBlendAmount = !FAnimWeight::IsFullWeight(LayeringState.ArmRightLocalSpaceBlendAmount);

	LayeringState.HandLeftBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerHandLeftCurve());
	LayeringState.HandRightBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerHandRightCurve());

	LayeringState.SpineBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerSpineCurve());
	LayeringState.SpineAdditiveBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerSpineAdditiveCurve());
	LayeringState.SpineSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerSpineSlotCurve());

	LayeringState.PelvisBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerPelvisCurve());
	LayeringState.PelvisSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerPelvisSlotCurve());

	LayeringState.LegsBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerLegsCurve());
	LayeringState.LegsSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerLegsSlotCurve());
}

void UAlsAnimationInstance::RefreshPose()
{
	if (!bAnimationCurvesRelevant)
	{
		return;
	}

	PoseState.GaitAmount = FMath::Clamp(GetCurveValue(UAlsConstants::PoseGaitCurve()), 0.0f, 3.0f);
	PoseState.GaitWalkingAmount = UAlsMath::Clamp01(PoseState.GaitAmount);
	PoseState.GaitRunningAmount = UAlsMath::Clamp01(PoseState.GaitAmount - 1.0f);
	PoseState.GaitSprintingAmount = UAlsMath::Clamp01(PoseState.GaitAmount - 2.0f);

	PoseState.MovingAmount = GetCurveValueClamped01(UAlsConstants::PoseMovingCurve());

	PoseState.StandingAmount = GetCurveValueClamped01(UAlsConstants::PoseStandingCurve());
	PoseState.CrouchingAmount = GetCurveValueClamped01(UAlsConstants::PoseCrouchingCurve());

	PoseState.GroundedAmount = GetCurveValueClamped01(UAlsConstants::PoseGroundedCurve());
	PoseState.InAirAmount = GetCurveValueClamped01(UAlsConstants::PoseInAirCurve());
}

void UAlsAnimationInstance::RefreshViewGameThread()
{
	check(IsInGameThread())

	const auto& View{Character->GetViewState()};

	ViewState.Rotation = View.Rotation;
	ViewState.YawSpeed = View.YawSpeed;
}

void UAlsAnimationInstance::RefreshView(const float DeltaTime)
{
	if (!LocomotionAction.IsValid())
	{
		ViewState.YawAngle = FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT(ViewState.Rotation.Yaw - LocomotionState.Rotation.Yaw));
		ViewState.PitchAngle = FRotator3f::NormalizeAxis(UE_REAL_TO_FLOAT(ViewState.Rotation.Pitch - LocomotionState.Rotation.Pitch));
	}

	if (!RotationMode.IsVelocityDirection())
	{
		ViewState.PitchAmount = FMath::GetMappedRangeValueClamped(FVector2f{-90.0f, 90.0f}, {1.0f, 0.0f}, ViewState.PitchAngle);
	}

	// Interpolate the view rotation value to achieve smooth view rotation changes. Interpolating
	// the rotation before calculating the angle ensures the value is not affected by changes in
	// actor rotation, allowing slow view rotation changes with fast actor rotation changes.

	ViewState.SmoothRotation = bPendingUpdate
		                           ? ViewState.Rotation
		                           : UAlsMath::ExponentialDecay(ViewState.SmoothRotation, ViewState.Rotation, DeltaTime,
		                                                        Settings->General.ViewSmoothRotationInterpolationSpeed);

	ViewState.SmoothYawAngle = FRotator3f::NormalizeAxis(
		UE_REAL_TO_FLOAT(ViewState.SmoothRotation.Yaw - LocomotionState.Rotation.Yaw));

	ViewState.SmoothPitchAngle = FRotator3f::NormalizeAxis(
		UE_REAL_TO_FLOAT(ViewState.SmoothRotation.Pitch - LocomotionState.Rotation.Pitch));

	// Separate the smooth view yaw angle into 3 separate values. These 3 values are used to
	// improve the blending of the view when rotating completely around the character. This allows
	// you to keep the view responsive but still smoothly blend from left to right or right to left.

	ViewState.SmoothYawAmount = (ViewState.SmoothYawAngle / 180.0f + 1.0f) * 0.5f;

	ViewState.SmoothYawLeftAmount = FMath::GetMappedRangeValueClamped(FVector2f{0.0f, 180.0f}, {0.5f, 0.0f},
	                                                                  FMath::Abs(ViewState.SmoothYawAngle));

	ViewState.SmoothYawRightAmount = FMath::GetMappedRangeValueClamped(FVector2f{0.0f, 180.0f}, {0.5f, 1.0f},
	                                                                   FMath::Abs(ViewState.SmoothYawAngle));

	const auto AimingAllowedAmount{1.0f - GetCurveValueClamped01(UAlsConstants::AimBlockCurve())};
	const auto AimingManualAmount{GetCurveValueClamped01(UAlsConstants::AimManualCurve())};

	ViewState.LookAmount = AimingAllowedAmount * (1.0f - AimingManualAmount);

	if (IsSpineRotationAllowed())
	{
		ViewState.TargetSpineYawAngle = ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
			                                ? ViewState.YawAngle - 360.0f
			                                : ViewState.YawAngle;
	}

	ViewState.SpineYawAngle = FRotator3f::NormalizeAxis(ViewState.TargetSpineYawAngle * AimingAllowedAmount * AimingManualAmount);
}

bool UAlsAnimationInstance::IsSpineRotationAllowed()
{
	return RotationMode.IsAiming();
}

void UAlsAnimationInstance::RefreshLocomotionGameThread()
{
	check(IsInGameThread())

	const auto& Locomotion{Character->GetLocomotionState()};

	LocomotionState.bHasInput = Locomotion.bHasInput;
	LocomotionState.InputYawAngle = Locomotion.InputYawAngle;

	LocomotionState.bHasSpeed = Locomotion.bHasSpeed;
	LocomotionState.Speed = Locomotion.Speed;
	LocomotionState.Velocity = Locomotion.Velocity;
	LocomotionState.VelocityYawAngle = Locomotion.VelocityYawAngle;
	LocomotionState.Acceleration = Locomotion.Acceleration;
	LocomotionState.MaxAcceleration = Character->GetCharacterMovement()->GetMaxAcceleration();
	LocomotionState.MaxBrakingDeceleration = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	LocomotionState.bMoving = Locomotion.bMoving;
	LocomotionState.Location = Locomotion.Location;
	LocomotionState.Rotation = Locomotion.Rotation;
	LocomotionState.RotationQuaternion = Locomotion.RotationQuaternion;
	LocomotionState.Scale = UE_REAL_TO_FLOAT(GetSkelMeshComponent()->GetComponentScale().Z);

	const auto* Capsule{Character->GetCapsuleComponent()};

	LocomotionState.CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	LocomotionState.CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	const auto& BasedMovement{Character->GetBasedMovement()};

	if (BasedMovement.MovementBase != LocomotionState.BasedMovement.Primitive ||
	    BasedMovement.BoneName != LocomotionState.BasedMovement.BoneName)
	{
		LocomotionState.BasedMovement.Primitive = BasedMovement.MovementBase;
		LocomotionState.BasedMovement.BoneName = BasedMovement.BoneName;
		LocomotionState.BasedMovement.bBaseChanged = true;
	}
	else
	{
		LocomotionState.BasedMovement.bBaseChanged = false;
	}

	LocomotionState.BasedMovement.bHasRelativeLocation = BasedMovement.HasRelativeLocation();

	MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName,
	                                              LocomotionState.BasedMovement.Location, LocomotionState.BasedMovement.Rotation);
}

void UAlsAnimationInstance::RefreshLocomotion(const float DeltaTime)
{
	if (LocomotionState.bHasInput && RotationMode.IsVelocityDirection())
	{
		// Get the delta between character rotation and current input yaw angle and map it to a range from
		// 0 to 1. This value is used in the aiming to make the character look toward the current input.

		const auto InputYawAngle{FRotator3f::NormalizeAxis(LocomotionState.InputYawAngle - UE_REAL_TO_FLOAT(LocomotionState.Rotation.Yaw))};

		// Convert [-180, 180] range to [0, 1].

		const auto InputYawAmount{(InputYawAngle / 180.0f + 1.0f) * 0.5f};

		LocomotionState.InputYawAmount = bPendingUpdate
			                                 ? InputYawAmount
			                                 : UAlsMath::ExponentialDecay(LocomotionState.InputYawAmount, InputYawAmount, DeltaTime,
			                                                              Settings->General.InputYawAmountInterpolationSpeed);
	}
	else
	{
		LocomotionState.InputYawAmount = bPendingUpdate
			                                 ? 0.5f
			                                 : UAlsMath::ExponentialDecay(LocomotionState.InputYawAmount, 0.5f, DeltaTime,
			                                                              Settings->General.InputYawAmountInterpolationSpeed);
	}

	// ReSharper disable once CppRedundantParentheses
	LocomotionState.bMovingSmooth = (LocomotionState.bHasInput && LocomotionState.bHasSpeed) ||
	                                LocomotionState.Speed > Settings->General.MovingSmoothSpeedThreshold;
}

void UAlsAnimationInstance::RefreshGroundedGameThread()
{
	check(IsInGameThread())

	GroundedState.bPivotActive = GroundedState.bPivotActivationRequested && !bPendingUpdate;
	GroundedState.bPivotActivationRequested = false;
}

void UAlsAnimationInstance::RefreshGrounded(const float DeltaTime)
{
	// Always sample sprint block curve, otherwise issues with inertial blending may occur.

	GroundedState.SprintBlockAmount = GetCurveValueClamped01(UAlsConstants::SprintBlockCurve());
	GroundedState.HipsDirectionLockAmount = FMath::Clamp(GetCurveValue(UAlsConstants::HipsDirectionLockCurve()), -1.0f, 1.0f);

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
	// relative to the actor rotation. It is normalized to a range of -1 to 1 so that -1 equals the
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
	// Calculate the movement direction. This value represents the direction the character is moving relative to the camera during
	// the looking direction / aiming modes and is used in the cycle blending to blend to the appropriate directional states.

	if (Gait.IsSprinting() || RotationMode.IsVelocityDirection())
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

	// Calculate and interpolate the velocity blend. This value represents the velocity amount of the
	// actor in each direction (normalized so that diagonals equal 0.5 for each direction) and is
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
	if (!Gait.IsSprinting())
	{
		GroundedState.SprintTime = 0.0f;
		GroundedState.SprintAccelerationAmount = 0.0f;
		return;
	}

	// Use the relative acceleration as the sprint relative acceleration if less than 0.5 seconds has elapsed
	// since the start of the sprint, otherwise set the sprint relative acceleration to zero.This is
	// necessary in order to apply the acceleration animation only at the beginning of the sprint.

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
	// Calculate the stride blend. This value is used within the blend spaces to scale the stride (distance feet travel) so
	// that the character can walk or run at different movement speeds. It also allows the walk or run gait animations to
	// blend independently while still matching the animation speed to the movement speed, preventing the character from needing
	// to play a half walk + half run blend. The curves are used to map the stride amount to the speed for maximum control.

	const auto Speed{LocomotionState.Speed / LocomotionState.Scale};

	const auto StandingStrideBlend{
		FMath::Lerp(Settings->Grounded.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
		            Settings->Grounded.StrideBlendAmountRunCurve->GetFloatValue(Speed),
		            PoseState.GaitRunningAmount)
	};

	// Crouching stride blend.

	GroundedState.StrideBlendAmount = FMath::Lerp(StandingStrideBlend,
	                                              Settings->Grounded.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
	                                              PoseState.CrouchingAmount);
}

void UAlsAnimationInstance::RefreshWalkRunBlendAmount()
{
	// Calculate the walk run blend. This value is used within the blend spaces to blend between walking and running.

	GroundedState.WalkRunBlendAmount = Gait.IsWalking() ? 0.0f : 1.0f;
}

void UAlsAnimationInstance::RefreshStandingPlayRate()
{
	// Calculate the standing play rate by dividing the character's speed by the animated speed for each gait.
	// The interpolation are determined by the gait amount curve that exists on every locomotion cycle so that
	// the play rate is always in sync with the currently blended animation. The value is also divided by the
	// stride blend and the capsule scale so that the play rate increases as the stride or scale gets smaller.

	const auto WalkRunSpeedAmount{
		FMath::Lerp(LocomotionState.Speed / Settings->Grounded.AnimatedWalkSpeed,
		            LocomotionState.Speed / Settings->Grounded.AnimatedRunSpeed,
		            PoseState.GaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount{
		FMath::Lerp(WalkRunSpeedAmount,
		            LocomotionState.Speed / Settings->Grounded.AnimatedSprintSpeed,
		            PoseState.GaitSprintingAmount)
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
	LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, 0.0f, DeltaTime, Settings->General.LeanInterpolationSpeed);
	LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, 0.0f, DeltaTime, Settings->General.LeanInterpolationSpeed);
}

void UAlsAnimationInstance::RefreshInAirGameThread()
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

	RefreshGroundPredictionAmount();

	RefreshInAirLeanAmount(DeltaTime);
}

void UAlsAnimationInstance::RefreshGroundPredictionAmount()
{
	// Calculate the ground prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward and getting the "time" (range from 0 to 1, 1 being maximum, 0 being about to ground) till impact.
	// The ground prediction amount curve is used to control how the time affects the final amount for a smooth blend.

	static constexpr auto VerticalVelocityThreshold{-200.0f};

	if (LocomotionState.Velocity.Z > VerticalVelocityThreshold)
	{
		InAirState.GroundPredictionAmount = 0.0f;
		return;
	}

	const auto AllowanceAmount{1.0f - GetCurveValueClamped01(UAlsConstants::GroundPredictionBlockCurve())};
	if (AllowanceAmount <= KINDA_SMALL_NUMBER)
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
		                                                      LocomotionState.Velocity.Z) * LocomotionState.Scale
	};

	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : Settings->InAir.GroundPredictionSweepObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}

	FHitResult Hit;
	GetWorld()->SweepSingleByObjectType(Hit, SweepStartLocation, SweepStartLocation + SweepVector, FQuat::Identity, ObjectQueryParameters,
	                                    FCollisionShape::MakeCapsule(LocomotionState.CapsuleRadius, LocomotionState.CapsuleHalfHeight),
	                                    {ANSI_TO_TCHAR(__FUNCTION__), false, Character});

	static constexpr auto MaxSlopeCos{0.71f}; // cos 0.71 = 45 degrees.

	const auto bGroundValid{Hit.IsValidBlockingHit() && Hit.ImpactNormal.Z >= MaxSlopeCos};

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
		FVector3f{LocomotionState.RotationQuaternion.UnrotateVector(LocomotionState.Velocity)} / ReferenceSpeed *
		Settings->InAir.LeanAmountCurve->GetFloatValue(UE_REAL_TO_FLOAT(LocomotionState.Velocity.Z))
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

void UAlsAnimationInstance::RefreshFeetGameThread()
{
	check(IsInGameThread())

	const auto* Mesh{GetSkelMeshComponent()};

	const auto FootLeftTransform{
		Mesh->GetSocketTransform(Settings->General.bUseFootIkBones
			                         ? UAlsConstants::FootLeftIkBone()
			                         : UAlsConstants::FootLeftVirtualBone())
	};

	FeetState.Left.BoneLocation = FootLeftTransform.GetLocation();
	FeetState.Left.BoneRotation = FootLeftTransform.GetRotation();

	const auto FootRightTransform{
		Mesh->GetSocketTransform(Settings->General.bUseFootIkBones
			                         ? UAlsConstants::FootRightIkBone()
			                         : UAlsConstants::FootRightVirtualBone())
	};

	FeetState.Right.BoneLocation = FootRightTransform.GetLocation();
	FeetState.Right.BoneRotation = FootRightTransform.GetRotation();
}

void UAlsAnimationInstance::RefreshFeet(const float DeltaTime)
{
	FeetState.bReinitializationRequired |= bPendingUpdate || !bAnimationCurvesRelevant;

	// If animation curves are not relevant, then skip feet update entirely.

	if (!bAnimationCurvesRelevant)
	{
		return;
	}

	FeetState.FootPlantedAmount = FMath::Clamp(GetCurveValue(UAlsConstants::FootPlantedCurve()), -1.0f, 1.0f);
	FeetState.FeetCrossingAmount = GetCurveValueClamped01(UAlsConstants::FeetCrossingCurve());

	const auto ComponentTransformInverse{GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().Inverse()};

	FVector TargetFootLeftLocationOffset;
	RefreshFoot(FeetState.Left, UAlsConstants::FootLeftIkCurve(), UAlsConstants::FootLeftLockCurve(),
	            ComponentTransformInverse, DeltaTime, TargetFootLeftLocationOffset);

	FVector TargetFootRightLocationOffset;
	RefreshFoot(FeetState.Right, UAlsConstants::FootRightIkCurve(), UAlsConstants::FootRightLockCurve(),
	            ComponentTransformInverse, DeltaTime, TargetFootRightLocationOffset);

	RefreshPelvisOffset(UE_REAL_TO_FLOAT(TargetFootLeftLocationOffset.Z), UE_REAL_TO_FLOAT(TargetFootRightLocationOffset.Z), DeltaTime);

	FeetState.bReinitializationRequired = false;
}

void UAlsAnimationInstance::RefreshFoot(FAlsFootState& FootState, const FName& FootIkCurveName,
                                        const FName& FootLockCurveName, const FTransform& ComponentTransformInverse,
                                        const float DeltaTime, FVector& TargetLocationOffset) const
{
	FootState.IkAmount = GetCurveValueClamped01(FootIkCurveName);

	ProcessFootLockTeleport(FootState);

	ProcessFootLockBaseChange(FootState, ComponentTransformInverse);

	auto FinalLocation{FootState.BoneLocation};
	auto FinalRotation{FootState.BoneRotation};

	RefreshFootLock(FootState, FootLockCurveName, ComponentTransformInverse, DeltaTime, FinalLocation, FinalRotation);

	RefreshFootOffset(FootState, DeltaTime, TargetLocationOffset, FinalLocation, FinalRotation);

	FootState.IkLocation = ComponentTransformInverse.TransformPosition(FinalLocation);
	FootState.IkRotation = ComponentTransformInverse.TransformRotation(FinalRotation);
}

void UAlsAnimationInstance::ProcessFootLockTeleport(FAlsFootState& FootState) const
{
	if (!bTeleported || FeetState.bReinitializationRequired ||
	    !FAnimWeight::IsRelevant(FootState.IkAmount * FootState.LockAmount))
	{
		return;
	}

	const auto& ComponentTransform{GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform()};

	FootState.LockLocation = ComponentTransform.TransformPosition(FootState.LockComponentRelativeLocation);
	FootState.LockRotation = ComponentTransform.TransformRotation(FootState.LockComponentRelativeRotation);

	if (LocomotionState.BasedMovement.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{LocomotionState.BasedMovement.Rotation.Inverse()};

		FootState.LockMovementBaseRelativeLocation = BaseRotationInverse
			.RotateVector(FootState.LockLocation - LocomotionState.BasedMovement.Location);

		FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FootState.LockRotation;
	}
}

void UAlsAnimationInstance::ProcessFootLockBaseChange(FAlsFootState& FootState, const FTransform& ComponentTransformInverse) const
{
	if (!LocomotionState.BasedMovement.bBaseChanged && !FeetState.bReinitializationRequired ||
	    !FAnimWeight::IsRelevant(FootState.IkAmount * FootState.LockAmount))
	{
		return;
	}

	if (FeetState.bReinitializationRequired)
	{
		FootState.LockLocation = FootState.BoneLocation;
		FootState.LockRotation = FootState.BoneRotation;
	}

	FootState.LockComponentRelativeLocation = ComponentTransformInverse.TransformPosition(FootState.LockLocation);
	FootState.LockComponentRelativeRotation = ComponentTransformInverse.TransformRotation(FootState.LockRotation);

	if (LocomotionState.BasedMovement.bHasRelativeLocation)
	{
		const auto BaseRotationInverse{LocomotionState.BasedMovement.Rotation.Inverse()};

		FootState.LockMovementBaseRelativeLocation = BaseRotationInverse
			.RotateVector(FootState.LockLocation - LocomotionState.BasedMovement.Location);

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

	if (LocomotionState.bMovingSmooth || LocomotionMode != AlsLocomotionModeTags::Grounded)
	{
		// Smoothly disable foot locking if the character is moving or in the air,
		// instead of relying on the curve value from the animation blueprint.

		static constexpr auto MovingDecreaseSpeed{5.0f};
		static constexpr auto InAirDecreaseSpeed{0.6f};

		NewFootLockAmount = FeetState.bReinitializationRequired
			                    ? 0.0f
			                    : FMath::Max(0.0f, FMath::Min(NewFootLockAmount,
			                                                  FootState.LockAmount - DeltaTime *
			                                                  (LocomotionState.bMovingSmooth ? MovingDecreaseSpeed : InAirDecreaseSpeed)));
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

	const auto bNewAmountIsEqualOne{FAnimWeight::IsFullWeight(NewFootLockAmount)};
	const auto bNewAmountIsLessThanPrevious{NewFootLockAmount <= FootState.LockAmount};

	// Update the foot lock amount only if the new amount is less than the current amount or equal to 1. This
	// allows the foot to blend out from a locked location or lock to a new location, but never blend in.

	if (bNewAmountIsEqualOne || bNewAmountIsLessThanPrevious)
	{
		FootState.LockAmount = NewFootLockAmount;

		// If the new foot lock amount is 1 and the previous amount is less than 1, then save the new foot lock location and rotation.

		if (bNewAmountIsEqualOne && !bNewAmountIsLessThanPrevious)
		{
			FootState.LockLocation = FootState.BoneLocation;
			FootState.LockRotation = FootState.BoneRotation;

			if (LocomotionState.BasedMovement.bHasRelativeLocation)
			{
				const auto BaseRotationInverse{LocomotionState.BasedMovement.Rotation.Inverse()};

				FootState.LockMovementBaseRelativeLocation = BaseRotationInverse.
					RotateVector(FootState.BoneLocation - LocomotionState.BasedMovement.Location);

				FootState.LockMovementBaseRelativeRotation = BaseRotationInverse * FootState.BoneRotation;
			}
			else
			{
				FootState.LockMovementBaseRelativeLocation = FVector::ZeroVector;
				FootState.LockMovementBaseRelativeRotation = FQuat::Identity;
			}
		}
	}

	if (LocomotionState.BasedMovement.bHasRelativeLocation)
	{
		FootState.LockLocation = LocomotionState.BasedMovement.Location +
		                         LocomotionState.BasedMovement.Rotation.RotateVector(FootState.LockMovementBaseRelativeLocation);

		FootState.LockRotation = LocomotionState.BasedMovement.Rotation * FootState.LockMovementBaseRelativeRotation;
	}

	FootState.LockComponentRelativeLocation = ComponentTransformInverse.TransformPosition(FootState.LockLocation);
	FootState.LockComponentRelativeRotation = ComponentTransformInverse.TransformRotation(FootState.LockRotation);

	FinalLocation = FMath::Lerp(FinalLocation, FootState.LockLocation, FootState.LockAmount);
	FinalRotation = FQuat::Slerp(FinalRotation, FootState.LockRotation, FootState.LockAmount);
}

void UAlsAnimationInstance::RefreshFootOffset(FAlsFootState& FootState, const float DeltaTime, FVector& TargetLocationOffset,
                                              FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FAnimWeight::IsRelevant(FootState.IkAmount))
	{
		FootState.OffsetSpringState.Reset();
		TargetLocationOffset = FVector::ZeroVector;
		return;
	}

	if (LocomotionMode == AlsLocomotionModeTags::InAir)
	{
		FootState.OffsetSpringState.Reset();
		TargetLocationOffset = FVector::ZeroVector;

		if (FeetState.bReinitializationRequired)
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

	auto FootLocation{FinalLocation};
	FootLocation.Z = GetProxyOnAnyThread<FAnimInstanceProxy>().GetComponentTransform().GetLocation().Z;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit,
	                                     FootLocation + FVector{0.0f, 0.0f, Settings->Feet.IkTraceDistanceUpward * LocomotionState.Scale},
	                                     FootLocation - FVector{0.0f, 0.0f, Settings->Feet.IkTraceDistanceDownward * LocomotionState.Scale},
	                                     UEngineTypes::ConvertToCollisionChannel(Settings->Feet.IkTraceChannel),
	                                     {ANSI_TO_TCHAR(__FUNCTION__), true, Character});

	static constexpr auto MaxSlopeCos{0.71f}; // cos 0.71 = 45 degrees.

	const auto bGroundValid{Hit.IsValidBlockingHit() && Hit.ImpactNormal.Z >= MaxSlopeCos};

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
			const_cast<TArray<TFunction<void()>>&>(DisplayDebugTracesQueue).Add([this, Hit, bGroundValid]
				{
					UAlsUtility::DrawDebugLineTraceSingle(GetWorld(), Hit.TraceStart, Hit.TraceEnd, bGroundValid,
					                                      Hit, {0.0f, 0.25f, 1.0f}, {0.0f, 0.75f, 1.0f});
				}
			);
		}
	}
#endif

	FQuat TargetRotationOffset;

	if (bGroundValid)
	{
		const auto FootHeight{Settings->Feet.FootHeight * LocomotionState.Scale};

		// Find the difference in location from the impact location and the expected (flat) floor location. These values
		// are offset by the impact normal multiplied by the foot height to get better behavior on angled surfaces.

		TargetLocationOffset = Hit.ImpactPoint - FootLocation + Hit.ImpactNormal * FootHeight;
		TargetLocationOffset.Z -= FootHeight;

		// Calculate the rotation offset.

		TargetRotationOffset = FRotator{
			-UAlsMath::DirectionToAngle({Hit.ImpactNormal.Z, Hit.ImpactNormal.X}),
			0.0f,
			UAlsMath::DirectionToAngle({Hit.ImpactNormal.Z, Hit.ImpactNormal.Y})
		}.Quaternion();
	}
	else
	{
		TargetLocationOffset = FVector::ZeroVector;
		TargetRotationOffset = FQuat::Identity;
	}

	// Interpolate current offsets to the new target values.

	if (FeetState.bReinitializationRequired)
	{
		FootState.OffsetSpringState.Reset();

		FootState.OffsetLocation = TargetLocationOffset;
		FootState.OffsetRotation = TargetRotationOffset;
	}
	else
	{
		static constexpr auto LocationInterpolationFrequency{0.4f};
		static constexpr auto LocationInterpolationDampingRatio{4.0f};
		static constexpr auto LocationInterpolationTargetVelocityAmount{1.0f};

		FootState.OffsetLocation = UAlsMath::SpringDamp(FootState.OffsetLocation, TargetLocationOffset,
		                                                FootState.OffsetSpringState, DeltaTime, LocationInterpolationFrequency,
		                                                LocationInterpolationDampingRatio, LocationInterpolationTargetVelocityAmount);

		static constexpr auto RotationInterpolationSpeed{30.0f};

		FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, TargetRotationOffset, DeltaTime, RotationInterpolationSpeed);
	}

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;
}

void UAlsAnimationInstance::RefreshPelvisOffset(const float TargetFootLeftLocationOffsetZ, const float TargetFootRightLocationOffsetZ,
                                                const float DeltaTime)
{
	// Calculate the pelvis offset amount by finding the average foot ik weight.

	const auto OffsetAmount{(FeetState.Left.IkAmount + FeetState.Right.IkAmount) * 0.5f};

	if (!FAnimWeight::IsRelevant(OffsetAmount))
	{
		FeetState.PelvisSpringState.Reset();
		FeetState.PelvisOffsetZ = 0.0f;
		return;
	}

	// Set the new offset to be the lowest foot offset.

	const auto NewTargetOffset{FMath::Min(TargetFootLeftLocationOffsetZ, TargetFootRightLocationOffsetZ) / LocomotionState.Scale};

	// Interpolate current offset to the new target value.

	if (FeetState.bReinitializationRequired)
	{
		FeetState.PelvisSpringState.Reset();
		FeetState.TargetPelvisOffsetZ = NewTargetOffset;
	}
	else
	{
		static constexpr auto InterpolationFrequency{0.2f};
		static constexpr auto InterpolationDampingRatio{2.0f};
		static constexpr auto InterpolationTargetVelocityAmount{0.5f};

		FeetState.TargetPelvisOffsetZ = UAlsMath::SpringDamp(FeetState.TargetPelvisOffsetZ, NewTargetOffset,
		                                                     FeetState.PelvisSpringState, DeltaTime, InterpolationFrequency,
		                                                     InterpolationDampingRatio, InterpolationTargetVelocityAmount);
	}

	FeetState.PelvisOffsetZ = FeetState.TargetPelvisOffsetZ * OffsetAmount;
}

void UAlsAnimationInstance::PlayTransitionAnimation(UAnimSequenceBase* Animation, const float BlendInTime, const float BlendOutTime,
                                                    const float PlayRate, const float StartTime, const bool bFromStandingIdleOnly)
{
	check(IsInGameThread())

	// ReSharper disable once CppRedundantParentheses
	if ((bFromStandingIdleOnly && (Character->GetLocomotionState().bMoving || Character->GetStance() != EAlsStance::Standing)))
	{
		return;
	}

	PlaySlotAnimationAsDynamicMontage(Animation, UAlsConstants::TransitionSlot(), BlendInTime, BlendOutTime, PlayRate, 1, 0.0f, StartTime);
}

void UAlsAnimationInstance::PlayTransitionLeftAnimation(const float BlendInTime, const float BlendOutTime, const float PlayRate,
                                                        const float StartTime, const bool bFromStandingIdleOnly)
{
	PlayTransitionAnimation(Stance.IsCrouching()
		                        ? Settings->Transitions.CrouchingTransitionLeftAnimation
		                        : Settings->Transitions.StandingTransitionLeftAnimation,
	                        BlendInTime, BlendOutTime, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UAlsAnimationInstance::PlayTransitionRightAnimation(const float BlendInTime, const float BlendOutTime, const float PlayRate,
                                                         const float StartTime, const bool bFromStandingIdleOnly)
{
	PlayTransitionAnimation(Stance.IsCrouching()
		                        ? Settings->Transitions.CrouchingTransitionRightAnimation
		                        : Settings->Transitions.StandingTransitionRightAnimation,
	                        BlendInTime, BlendOutTime, PlayRate, StartTime, bFromStandingIdleOnly);
}

void UAlsAnimationInstance::StopTransitionAndTurnInPlaceAnimations(const float BlendOutTime)
{
	check(IsInGameThread())

	StopSlotAnimation(BlendOutTime, UAlsConstants::TransitionSlot());
	StopSlotAnimation(BlendOutTime, UAlsConstants::TurnInPlaceStandingSlot());
	StopSlotAnimation(BlendOutTime, UAlsConstants::TurnInPlaceCrouchingSlot());
}

void UAlsAnimationInstance::RefreshTransitionsGameThread()
{
	check(IsInGameThread())

	PlayPQueuedDynamicTransitionAnimation();
}

void UAlsAnimationInstance::RefreshTransitions()
{
	// The allow transitions curve is modified within certain states, so that allow transition will be true while in those states.

	TransitionsState.bTransitionsAllowed = FAnimWeight::IsFullWeight(GetCurveValue(UAlsConstants::AllowTransitionsCurve()));

	RefreshDynamicTransition();
}

void UAlsAnimationInstance::RefreshDynamicTransition()
{
	if (TransitionsState.DynamicTransitionsFrameDelay > 0)
	{
		TransitionsState.DynamicTransitionsFrameDelay -= 1;
		return;
	}

	if (!bAnimationCurvesRelevant || !TransitionsState.bTransitionsAllowed ||
	    LocomotionState.bMoving || LocomotionMode != AlsLocomotionModeTags::Grounded)
	{
		return;
	}

	// Check each foot to see if the location difference between the foot look and its desired / target location
	// exceeds a threshold. If it does, play an additive transition animation on that foot. The currently set
	// transition plays the second half of a 2 foot transition animation, so that only a single foot moves.

	const auto FootLockDistanceThresholdSquared{
		FMath::Square(Settings->Transitions.DynamicTransitionFootLockDistanceThreshold * LocomotionState.Scale)
	};

	const auto FootLockLeftDistanceSquared{FVector::DistSquared(FeetState.Left.BoneLocation, FeetState.Left.LockLocation)};
	const auto FootLockRightDistanceSquared{FVector::DistSquared(FeetState.Right.BoneLocation, FeetState.Right.LockLocation)};

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

	// If both transitions are allowed, choose the one with the greater lock distance.

	if (!bTransitionLeftAllowed)
	{
		DynamicTransitionAnimation = Stance.IsCrouching()
			                             ? Settings->Transitions.CrouchingDynamicTransitionRightAnimation
			                             : Settings->Transitions.StandingDynamicTransitionRightAnimation;
	}
	else if (!bTransitionRightAllowed)
	{
		DynamicTransitionAnimation = Stance.IsCrouching()
			                             ? Settings->Transitions.CrouchingDynamicTransitionLeftAnimation
			                             : Settings->Transitions.StandingDynamicTransitionLeftAnimation;
	}
	else if (FootLockLeftDistanceSquared >= FootLockRightDistanceSquared)
	{
		DynamicTransitionAnimation = Stance.IsCrouching()
			                             ? Settings->Transitions.CrouchingDynamicTransitionLeftAnimation
			                             : Settings->Transitions.StandingDynamicTransitionLeftAnimation;
	}
	else
	{
		DynamicTransitionAnimation = Stance.IsCrouching()
			                             ? Settings->Transitions.CrouchingDynamicTransitionRightAnimation
			                             : Settings->Transitions.StandingDynamicTransitionRightAnimation;
	}

	if (!DynamicTransitionAnimation.IsNull())
	{
		// Block next dynamic transitions for about 2 frames to give the animation blueprint some time to properly react to the animation.

		TransitionsState.DynamicTransitionsFrameDelay = 2;

		// Animation montages can't be played in the worker thread, so queue them up to play later in the game thead.

		TransitionsState.QueuedDynamicTransitionAnimation = DynamicTransitionAnimation;

		if (IsInGameThread())
		{
			PlayPQueuedDynamicTransitionAnimation();
		}
	}
}

void UAlsAnimationInstance::PlayPQueuedDynamicTransitionAnimation()
{
	check(IsInGameThread())

	PlaySlotAnimationAsDynamicMontage(TransitionsState.QueuedDynamicTransitionAnimation, UAlsConstants::TransitionSlot(),
	                                  Settings->Transitions.DynamicTransitionBlendTime, Settings->Transitions.DynamicTransitionBlendTime,
	                                  Settings->Transitions.DynamicTransitionPlayRate, 1, 0.0f);

	TransitionsState.QueuedDynamicTransitionAnimation = nullptr;
}

bool UAlsAnimationInstance::IsRotateInPlaceAllowed()
{
	return RotationMode.IsAiming() || ViewMode.IsFirstPerson();
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
			                              ? UE_REAL_TO_FLOAT(Settings->RotateInPlace.PlayRate.X)
			                              : FMath::FInterpTo(RotateInPlaceState.PlayRate,
			                                                 UE_REAL_TO_FLOAT(Settings->RotateInPlace.PlayRate.X),
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
			                              ? UE_REAL_TO_FLOAT(Settings->RotateInPlace.PlayRate.X)
			                              : FMath::FInterpTo(RotateInPlaceState.PlayRate,
			                                                 UE_REAL_TO_FLOAT(Settings->RotateInPlace.PlayRate.X),
			                                                 DeltaTime, PlayRateInterpolationSpeed);

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// If the character should be rotating, set the play rate to scale with the view yaw speed.
	// This makes the character rotate faster when moving the camera faster.

	const auto PlayRate{
		FMath::GetMappedRangeValueClamped(FVector2f{Settings->RotateInPlace.ReferenceViewYawSpeed},
		                                  FVector2f{Settings->RotateInPlace.PlayRate}, ViewState.YawSpeed)
	};

	RotateInPlaceState.PlayRate = bPendingUpdate
		                              ? PlayRate
		                              : FMath::FInterpTo(RotateInPlaceState.PlayRate, PlayRate,
		                                                 DeltaTime, PlayRateInterpolationSpeed);

	// Disable foot locking when rotating at a large angle or rotating too fast, otherwise the legs may twist in a spiral.

	static constexpr auto BlockInterpolationSpeed{5.0f};

	RotateInPlaceState.FootLockBlockAmount =
		Settings->RotateInPlace.bDisableFootLock
			? 0.0f
			: FMath::Abs(ViewState.YawAngle) > Settings->RotateInPlace.FootLockBlockViewYawAngleThreshold
			? 1.0f
			: ViewState.YawSpeed <= Settings->RotateInPlace.FootLockBlockViewYawSpeedThreshold
			? 0.0f
			: bPendingUpdate
			? 1.0f
			: FMath::FInterpTo(RotateInPlaceState.FootLockBlockAmount, 1.0f, DeltaTime, BlockInterpolationSpeed);
}

bool UAlsAnimationInstance::IsTurnInPlaceAllowed()
{
	return RotationMode.IsLookingDirection() && !ViewMode.IsFirstPerson();
}

void UAlsAnimationInstance::RefreshTurnInPlaceGameThread()
{
	check(IsInGameThread())

	PlayQueuedTurnInPlaceAnimation();
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

	// Check if the view yaw speed is below the threshold and if view yaw angle is outside of the
	// threshold. If so, begin counting the activation delay time. If not, reset the activation delay time.
	// This ensures the conditions remain true for a sustained period of time before turning in place.

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
		                                  FVector2f{Settings->TurnInPlace.ViewYawAngleToActivationDelay},
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

	if (Stance.IsStanding())
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceStandingSlot();

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
	else if (Stance.IsCrouching())
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceCrouchingSlot();

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

	if (IsValid(TurnInPlaceSettings) && ensure(!TurnInPlaceSettings->Animation.IsNull()))
	{
		// Animation montages can't be played in the worker thread, so queue them up to play later in the game thead.

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

	if (TurnInPlaceState.QueuedSettings.IsNull())
	{
		return;
	}

	const auto* TurnInPlaceSettings{TurnInPlaceState.QueuedSettings.Get()};

	PlaySlotAnimationAsDynamicMontage(TurnInPlaceSettings->Animation, TurnInPlaceState.QueuedSlotName,
	                                  Settings->TurnInPlace.BlendTime, Settings->TurnInPlace.BlendTime,
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

void UAlsAnimationInstance::RefreshRagdollingGameThread()
{
	check(IsInGameThread())

	if (LocomotionAction != AlsLocomotionActionTags::Ragdolling)
	{
		return;
	}

	// Scale the flail play rate by the root speed. The faster the ragdoll moves, the faster the character will flail.

	static constexpr auto ReferenceSpeed{1000.0f};

	RagdollingState.FlailPlayRate = UAlsMath::Clamp01(
		UE_REAL_TO_FLOAT(GetSkelMeshComponent()->GetPhysicsLinearVelocity(UAlsConstants::RootBone()).Size() / ReferenceSpeed));
}

void UAlsAnimationInstance::StopRagdolling()
{
	check(IsInGameThread())

	// Save a snapshot of the current ragdoll pose for use in animation graph to blend out of the ragdoll.

	SnapshotPose(RagdollingState.FinalRagdollPose);
}

void UAlsAnimationInstance::FinalizeRagdolling() const
{
	check(IsInGameThread())

	Character->FinalizeRagdolling();
}

float UAlsAnimationInstance::GetCurveValueClamped01(const FName& CurveName) const
{
	return UAlsMath::Clamp01(GetCurveValue(CurveName));
}
