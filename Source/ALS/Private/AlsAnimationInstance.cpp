#include "AlsAnimationInstance.h"

#include "AlsCharacter.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsAnimationInstanceSettings.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"
#include "Utility/GameplayTags/AlsLocomotionActionTags.h"
#include "Utility/GameplayTags/AlsLocomotionModeTags.h"

void UAlsAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AAlsCharacter>(GetOwningActor());
}

void UAlsAnimationInstance::NativeBeginPlay()
{
	check(IsValid(Settings));
	check(IsValid(Character));

	Character->GetCapsuleComponent()->TransformUpdated.AddWeakLambda(
		this, [&bJustTeleported = bJustTeleported](USceneComponent*, const EUpdateTransformFlags, const ETeleportType TeleportType)
		{
			bJustTeleported |= TeleportType != ETeleportType::None;
		});

	Super::NativeBeginPlay();
}

void UAlsAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(Settings) || !IsValid(Character))
	{
		return;
	}

	Stance = Character->GetStance();
	Gait = Character->GetGait();
	RotationMode = Character->GetRotationMode();
	ViewMode = Character->GetViewMode();

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

	RefreshRagdolling();

	Character->ApplyRotationYawSpeedFromAnimationInstance(DeltaTime);

	bPendingUpdate = false;
	bJustTeleported = false;
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
	LayeringState.ArmLeftLocalSpaceBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmLeftLocalSpaceCurve());
	LayeringState.ArmLeftMeshSpaceBlendAmount = !FAnimWeight::IsFullWeight(LayeringState.ArmLeftLocalSpaceBlendAmount);
	LayeringState.ArmLeftSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmLeftSlotCurve());

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmRightBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightCurve());
	LayeringState.ArmRightAdditiveBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightAdditiveCurve());
	LayeringState.ArmRightLocalSpaceBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightLocalSpaceCurve());
	LayeringState.ArmRightMeshSpaceBlendAmount = !FAnimWeight::IsFullWeight(LayeringState.ArmRightLocalSpaceBlendAmount);
	LayeringState.ArmRightSlotBlendAmount = GetCurveValueClamped01(UAlsConstants::LayerArmRightSlotCurve());

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

void UAlsAnimationInstance::RefreshView(const float DeltaTime)
{
	if (!Character->GetLocomotionAction().IsValid())
	{
		ViewState.YawAngle = FRotator::NormalizeAxis(Character->GetViewState().Rotation.Yaw -
		                                             Character->GetLocomotionState().Rotation.Yaw);

		ViewState.PitchAngle = FRotator::NormalizeAxis(Character->GetViewState().Rotation.Pitch -
		                                               Character->GetLocomotionState().Rotation.Pitch);
	}

	// Interpolate the view rotation value to achieve smooth view rotation changes. Interpolating
	// the rotation before calculating the angle ensures the value is not affected by changes in
	// actor rotation, allowing slow view rotation changes with fast actor rotation changes.

	ViewState.SmoothRotation = !bPendingUpdate
		                           ? UAlsMath::ExponentialDecay(ViewState.SmoothRotation, Character->GetViewState().Rotation,
		                                                        DeltaTime, Settings->General.ViewSmoothRotationInterpolationSpeed)
		                           : Character->GetViewState().Rotation;

	ViewState.SmoothYawAngle = FRotator::NormalizeAxis(ViewState.SmoothRotation.Yaw -
	                                                   Character->GetLocomotionState().Rotation.Yaw);

	ViewState.SmoothPitchAngle = FRotator::NormalizeAxis(ViewState.SmoothRotation.Pitch -
	                                                     Character->GetLocomotionState().Rotation.Pitch);

	// Separate the smooth view yaw angle into 3 separate values. These 3 values are used to
	// improve the blending of the view when rotating completely around the character. This allows
	// you to keep the view responsive but still smoothly blend from left to right or right to left.

	ViewState.SmoothYawAmount = (ViewState.SmoothYawAngle / 180.0f + 1.0f) * 0.5f;
	ViewState.SmoothYawLeftAmount = FMath::GetMappedRangeValueClamped({0.0f, 180.0f}, {0.5f, 0.0f},
	                                                                  FMath::Abs(ViewState.SmoothYawAngle));
	ViewState.SmoothYawRightAmount = FMath::GetMappedRangeValueClamped({0.0f, 180.0f}, {0.5f, 1.0f},
	                                                                   FMath::Abs(ViewState.SmoothYawAngle));

	if (!RotationMode.IsVelocityDirection())
	{
		ViewState.PitchAmount = FMath::GetMappedRangeValueClamped({-90.0f, 90.0f}, {1.0f, 0.0f}, ViewState.PitchAngle);
	}

	const auto AimAllowedAmount{1.0f - GetCurveValueClamped01(UAlsConstants::AimBlockCurve())};
	const auto AimManualAmount{GetCurveValueClamped01(UAlsConstants::AimManualCurve())};

	ViewState.LookAmount = AimAllowedAmount * (1.0f - AimManualAmount);

	if (IsSpineRotationAllowed())
	{
		ViewState.TargetSpineYawAngle = ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
			                                ? ViewState.YawAngle - 360.0f
			                                : ViewState.YawAngle;
	}

	ViewState.SpineYawAngle = FRotator::NormalizeAxis(ViewState.TargetSpineYawAngle * AimAllowedAmount * AimManualAmount);
}

bool UAlsAnimationInstance::IsSpineRotationAllowed()
{
	return RotationMode.IsAiming();
}

void UAlsAnimationInstance::RefreshLocomotion(const float DeltaTime)
{
	if (Character->GetLocomotionState().bHasInput && RotationMode.IsVelocityDirection())
	{
		// Get the delta between character rotation and current input yaw angle and map it to a range from
		// 0 to 1. This value is used in the aiming to make the character look toward the current input.

		const auto InputYawAngle{
			FRotator::NormalizeAxis(Character->GetLocomotionState().InputYawAngle - Character->GetLocomotionState().Rotation.Yaw)
		};

		const auto InputYawAmount{(InputYawAngle / 180.0f + 1.0f) * 0.5f};

		LocomotionState.InputYawAmount = !bPendingUpdate
			                                 ? UAlsMath::ExponentialDecay(LocomotionState.InputYawAmount, InputYawAmount, DeltaTime,
			                                                              Settings->General.InputYawAmountInterpolationSpeed)
			                                 : InputYawAmount;
	}

	LocomotionState.bMovingSmooth = Character->GetLocomotionState().bHasInput ||
	                                Character->GetLocomotionState().Speed > Settings->General.MovingSmoothSpeedThreshold;
}

void UAlsAnimationInstance::ActivatePivot()
{
	GroundedState.bPivotActive = Character->GetLocomotionState().Speed < Settings->Grounded.PivotActivationSpeedThreshold;

	if (GroundedState.bPivotActive)
	{
		static constexpr auto ResetDelay{0.1f};

		GetWorld()->GetTimerManager().SetTimer(GroundedState.PivotResetTimer,
		                                       FTimerDelegate::CreateWeakLambda(this, [&bPivotActive = GroundedState.bPivotActive]
		                                       {
			                                       bPivotActive = false;
		                                       }), ResetDelay, false);
	}
}

void UAlsAnimationInstance::RefreshGrounded(const float DeltaTime)
{
	if (!Character->GetLocomotionState().bMoving || Character->GetLocomotionMode() != FAlsLocomotionModeTags::Get().Grounded)
	{
		return;
	}

	GroundedState.HipsDirectionLockAmount = FMath::Clamp(GetCurveValue(UAlsConstants::HipsDirectionLockCurve()), -1.0f, 1.0f);

	RefreshMovementDirection();
	RefreshVelocityBlend(DeltaTime);
	RefreshRotationYawOffsets();

	// Calculate the relative acceleration amount. This value represents the current amount of acceleration / deceleration
	// relative to the actor rotation. It is normalized to a range of -1 to 1 so that -1 equals the
	// max braking deceleration and 1 equals the max acceleration of the character movement component.

	const auto Acceleration{Character->GetLocomotionState().Acceleration};
	FVector RelativeAccelerationAmount;

	if ((Acceleration | Character->GetLocomotionState().Velocity) >= 0.0f)
	{
		RelativeAccelerationAmount = UAlsMath::ClampMagnitude01(
			Character->GetLocomotionState().RotationQuaternion.UnrotateVector(Acceleration) /
			Character->GetCharacterMovement()->GetMaxAcceleration());
	}
	else
	{
		RelativeAccelerationAmount = UAlsMath::ClampMagnitude01(
			Character->GetLocomotionState().RotationQuaternion.UnrotateVector(Acceleration) /
			Character->GetCharacterMovement()->GetMaxBrakingDeceleration());
	}

	// Use the relative acceleration as the sprint relative acceleration if less than 0.5 seconds has elapsed
	// since the start of the sprint, otherwise set the sprint relative acceleration to zero.This is
	// necessary in order to apply the acceleration animation only at the beginning of the sprint.

	if (Gait.IsSprinting())
	{
		static constexpr auto TimeThreshold{0.5f};

		GroundedState.SprintTime = !bPendingUpdate
			                           ? GroundedState.SprintTime + DeltaTime
			                           : TimeThreshold;

		GroundedState.SprintAccelerationAmount = GroundedState.SprintTime < TimeThreshold
			                                         ? RelativeAccelerationAmount.X
			                                         : 0.0f;
	}
	else
	{
		GroundedState.SprintTime = 0.0f;
		GroundedState.SprintAccelerationAmount = 0.0f;
	}

	GroundedState.SprintBlockAmount = GetCurveValueClamped01(UAlsConstants::SprintBlockCurve());

	GroundedState.StrideBlendAmount = CalculateStrideBlendAmount();
	GroundedState.WalkRunBlendAmount = CalculateWalkRunBlendAmount();

	GroundedState.StandingPlayRate = CalculateStandingPlayRate();
	GroundedState.CrouchingPlayRate = CalculateCrouchingPlayRate();

	// Interpolate the lean amount.

	if (!bPendingUpdate)
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, RelativeAccelerationAmount.Y,
		                                         DeltaTime, Settings->General.LeanInterpolationSpeed);

		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, RelativeAccelerationAmount.X,
		                                           DeltaTime, Settings->General.LeanInterpolationSpeed);
	}
	else
	{
		LeanState.RightAmount = RelativeAccelerationAmount.Y;
		LeanState.ForwardAmount = RelativeAccelerationAmount.X;
	}
}

void UAlsAnimationInstance::RefreshMovementDirection()
{
	// Calculate the movement direction. This value represents the direction the character is moving relative to the camera during
	// the looking direction / aiming modes and is used in the cycle blending to blend to the appropriate directional states.

	if (Gait.IsSprinting() || RotationMode.IsVelocityDirection())
	{
		GroundedState.MovementDirection = EAlsMovementDirection::Forward;
	}

	static constexpr auto ForwardHalfAngle{70.0f};

	GroundedState.MovementDirection = UAlsMath::CalculateMovementDirection(
		FRotator::NormalizeAxis(Character->GetLocomotionState().VelocityYawAngle - Character->GetViewState().Rotation.Yaw),
		ForwardHalfAngle, 5.0f);
}

void UAlsAnimationInstance::RefreshVelocityBlend(const float DeltaTime)
{
	// Calculate and interpolate the velocity blend. This value represents the velocity amount of the
	// actor in each direction (normalized so that diagonals equal 0.5 for each direction) and is
	// used in a blend multi node to produce better directional blending than a standard blend space.

	const auto RelativeVelocityDirection{
		Character->GetLocomotionState().RotationQuaternion.UnrotateVector(Character->GetLocomotionState().Velocity).GetSafeNormal()
	};

	const auto RelativeDirection{
		RelativeVelocityDirection /
		(FMath::Abs(RelativeVelocityDirection.X) +
		 FMath::Abs(RelativeVelocityDirection.Y) +
		 FMath::Abs(RelativeVelocityDirection.Z))
	};

	if (!bPendingUpdate)
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
	else
	{
		GroundedState.VelocityBlend.ForwardAmount = UAlsMath::Clamp01(RelativeDirection.X);
		GroundedState.VelocityBlend.BackwardAmount = FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f));
		GroundedState.VelocityBlend.LeftAmount = FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f));
		GroundedState.VelocityBlend.RightAmount = UAlsMath::Clamp01(RelativeDirection.Y);
	}
}

void UAlsAnimationInstance::RefreshRotationYawOffsets()
{
	// Set the rotation yaw offsets. These values influence the rotation yaw offset curve in the
	// animation graph and are used to offset the character's rotation for more natural movement.
	// The curves allow for fine control over how the offset behaves for each movement direction.

	const auto RotationYawOffset{
		FRotator::NormalizeAxis(Character->GetLocomotionState().VelocityYawAngle - Character->GetViewState().Rotation.Yaw)
	};

	GroundedState.RotationYawOffsets.ForwardAngle = Settings->Grounded.RotationYawOffsetForwardCurve->GetFloatValue(RotationYawOffset);
	GroundedState.RotationYawOffsets.BackwardAngle = Settings->Grounded.RotationYawOffsetBackwardCurve->GetFloatValue(RotationYawOffset);
	GroundedState.RotationYawOffsets.LeftAngle = Settings->Grounded.RotationYawOffsetLeftCurve->GetFloatValue(RotationYawOffset);
	GroundedState.RotationYawOffsets.RightAngle = Settings->Grounded.RotationYawOffsetRightCurve->GetFloatValue(RotationYawOffset);
}

float UAlsAnimationInstance::CalculateStrideBlendAmount() const
{
	// Calculate the stride blend. This value is used within the blend spaces to scale the stride (distance feet travel) so
	// that the character can walk or run at different movement speeds. It also allows the walk or run gait animations to
	// blend independently while still matching the animation speed to the movement speed, preventing the character from needing
	// to play a half walk + half run blend. The curves are used to map the stride amount to the speed for maximum control.

	const auto Speed{Character->GetLocomotionState().Speed / GetSkelMeshComponent()->GetComponentScale().Z};

	const auto StandingStrideBlend{
		FMath::Lerp(Settings->Grounded.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
		            Settings->Grounded.StrideBlendAmountRunCurve->GetFloatValue(Speed),
		            PoseState.GaitRunningAmount)
	};

	// Crouching stride blend.

	return FMath::Lerp(StandingStrideBlend,
	                   Settings->Grounded.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
	                   PoseState.CrouchingAmount);
}

float UAlsAnimationInstance::CalculateWalkRunBlendAmount() const
{
	// Calculate the walk run blend. This value is used within the blend spaces to blend between walking and running.

	return Gait.IsWalking() ? 0.0f : 1.0f;
}

float UAlsAnimationInstance::CalculateStandingPlayRate() const
{
	// Calculate the standing play rate by dividing the character's speed by the animated speed for each gait.
	// The interpolation are determined by the gait amount curve that exists on every locomotion cycle so that the
	// play rate is always in sync with the currently blended animation. The value is also divided by the
	// stride blend and the mesh scale so that the play rate increases as the stride or scale gets smaller.

	const auto WalkRunSpeedAmount{
		FMath::Lerp(Character->GetLocomotionState().Speed / Settings->Grounded.AnimatedWalkSpeed,
		            Character->GetLocomotionState().Speed / Settings->Grounded.AnimatedRunSpeed,
		            PoseState.GaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount{
		FMath::Lerp(WalkRunSpeedAmount,
		            Character->GetLocomotionState().Speed / Settings->Grounded.AnimatedSprintSpeed,
		            PoseState.GaitSprintingAmount)
	};

	return FMath::Clamp(WalkRunSprintSpeedAmount / (GroundedState.StrideBlendAmount * GetSkelMeshComponent()->GetComponentScale().Z),
	                    0.0f, 3.0f);
}

float UAlsAnimationInstance::CalculateCrouchingPlayRate() const
{
	// Calculate the crouching play rate by dividing the character's speed by the animated speed. This value needs
	// to be separate from the standing play rate to improve the blend from crouching to standing while in motion.

	return FMath::Clamp(Character->GetLocomotionState().Speed /
	                    (Settings->Grounded.AnimatedCrouchSpeed * GroundedState.StrideBlendAmount *
	                     GetSkelMeshComponent()->GetComponentScale().Z), 0.0f, 2.0f);
}

void UAlsAnimationInstance::Jump()
{
	static constexpr auto ReferenceSpeed{600.0f};
	static constexpr auto MinPlayRate{1.2f};
	static constexpr auto MaxPlayRate{1.5f};

	InAirState.bJumped = true;
	InAirState.JumpPlayRate = UAlsMath::LerpClamped(MinPlayRate, MaxPlayRate,
	                                                Character->GetLocomotionState().Speed / ReferenceSpeed);

	static constexpr auto ResetDelay{0.1f};

	GetWorld()->GetTimerManager().SetTimer(InAirState.JumpResetTimer,
	                                       FTimerDelegate::CreateWeakLambda(this, [&bJumped = InAirState.bJumped]
	                                       {
		                                       bJumped = false;
	                                       }), ResetDelay, false);
}

void UAlsAnimationInstance::RefreshInAir(const float DeltaTime)
{
	if (Character->GetLocomotionMode() != FAlsLocomotionModeTags::Get().InAir)
	{
		return;
	}

	// Set the ground prediction amount.

	InAirState.GroundPredictionAmount = CalculateGroundPredictionAmount();

	// Interpolate the lean amount.

	const auto TargetLeanAmount{CalculateInAirLeanAmount()};

	if (!bPendingUpdate)
	{
		LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, TargetLeanAmount.RightAmount,
		                                         DeltaTime, Settings->General.LeanInterpolationSpeed);
		LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, TargetLeanAmount.ForwardAmount,
		                                           DeltaTime, Settings->General.LeanInterpolationSpeed);
	}
	else
	{
		LeanState.RightAmount = TargetLeanAmount.RightAmount;
		LeanState.ForwardAmount = TargetLeanAmount.ForwardAmount;
	}
}

float UAlsAnimationInstance::CalculateGroundPredictionAmount() const
{
	// Calculate the ground prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward and getting the "time" (range from 0 to 1, 1 being maximum, 0 being about to ground) till impact.
	// The ground prediction amount curve is used to control how the time affects the final amount for a smooth blend.

	static constexpr auto VerticalVelocityThreshold{-200.0f};

	if (Character->GetLocomotionState().Velocity.Z > VerticalVelocityThreshold)
	{
		return 0.0f;
	}

	const auto AllowanceAmount{1.0f - GetCurveValueClamped01(UAlsConstants::GroundPredictionBlockCurve())};
	if (AllowanceAmount <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	const auto SweepStartLocation{Character->GetLocomotionState().Location};

	const auto* Capsule{Character->GetCapsuleComponent()};

	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	static constexpr auto MinVerticalVelocity{-4000.0f};
	static constexpr auto MaxVerticalVelocity{-200.0f};

	auto VelocityDirection{Character->GetLocomotionState().Velocity};
	VelocityDirection.Z = FMath::Clamp(Character->GetLocomotionState().Velocity.Z,
	                                   MinVerticalVelocity, MaxVerticalVelocity);
	VelocityDirection.Normalize();

	static constexpr auto MinSweepDistance{150.0f};
	static constexpr auto MaxSweepDistance{2000.0f};

	const auto SweepVector{
		VelocityDirection *
		FMath::GetMappedRangeValueClamped({MaxVerticalVelocity, MinVerticalVelocity},
		                                  {MinSweepDistance, MaxSweepDistance},
		                                  Character->GetLocomotionState().Velocity.Z) * GetSkelMeshComponent()->GetComponentScale().Z
	};

	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : Settings->InAir.GroundPredictionSweepObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}

	FHitResult Hit;
	GetWorld()->SweepSingleByObjectType(Hit, SweepStartLocation, SweepStartLocation + SweepVector, FQuat::Identity,
	                                    ObjectQueryParameters, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
	                                    {ANSI_TO_TCHAR(__FUNCTION__), false, Character});

#if ENABLE_DRAW_DEBUG
	if (UAlsUtility::ShouldDisplayDebug(Character, UAlsConstants::TracesDisplayName()))
	{
		UAlsUtility::DrawDebugSweepSingleCapsule(GetWorld(), Hit.TraceStart, Hit.TraceEnd, FRotator::ZeroRotator, CapsuleRadius,
		                                         CapsuleHalfHeight, Character->GetCharacterMovement()->IsWalkable(Hit),
		                                         Hit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f});
	}
#endif

	if (!Character->GetCharacterMovement()->IsWalkable(Hit))
	{
		return 0.0f;
	}

	return Settings->InAir.GroundPredictionAmountCurve->GetFloatValue(Hit.Time) * AllowanceAmount;
}

FAlsLeanState UAlsAnimationInstance::CalculateInAirLeanAmount() const
{
	// Use the relative velocity direction and amount to determine how much the character should lean
	// while in air. The lean amount curve gets the vertical velocity and is used as a multiplier to
	// smoothly reverse the leaning direction when transitioning from moving upwards to moving downwards.

	static constexpr auto ReferenceSpeed{350.0f};

	const auto RelativeVelocity{
		Character->GetLocomotionState().RotationQuaternion.UnrotateVector(Character->GetLocomotionState().Velocity) /
		ReferenceSpeed * Settings->InAir.LeanAmountCurve->GetFloatValue(Character->GetLocomotionState().Velocity.Z)
	};

	return {RelativeVelocity.Y, RelativeVelocity.X};
}

void UAlsAnimationInstance::RefreshFeet(const float DeltaTime)
{
	FeetState.bReinitializationRequired |= bPendingUpdate || !bAnimationCurvesRelevant;

	// If the curves are not relevant, then skip feet update entirely.

	if (!bAnimationCurvesRelevant)
	{
		return;
	}

	const auto& CapsuleTransform{Character->GetCapsuleComponent()->GetComponentTransform()};

	if (bJustTeleported)
	{
		ProcessFootLockTeleport(FeetState.Left, CapsuleTransform);
		ProcessFootLockTeleport(FeetState.Right, CapsuleTransform);
	}

	FeetState.FootPlantedAmount = FMath::Clamp(GetCurveValue(UAlsConstants::FootPlantedCurve()), -1.0f, 1.0f);
	FeetState.FeetCrossingAmount = GetCurveValueClamped01(UAlsConstants::FeetCrossingCurve());

	FeetState.Left.IkAmount = GetCurveValueClamped01(UAlsConstants::FootLeftIkCurve());
	FeetState.Right.IkAmount = GetCurveValueClamped01(UAlsConstants::FootRightIkCurve());

	const auto& FootLeftBone{
		Settings->General.bUseFootIkBones
			? UAlsConstants::FootLeftIkBone()
			: UAlsConstants::FootLeftIkVirtualBone()
	};

	const auto& FootRightBone{
		Settings->General.bUseFootIkBones
			? UAlsConstants::FootRightIkBone()
			: UAlsConstants::FootRightIkVirtualBone()
	};

	const auto CapsuleRelativeTransform{CapsuleTransform.Inverse()};

	const auto& BasedMovement{Character->GetBasedMovement()};
	if (FeetState.bReinitializationRequired || BasedMovement.MovementBase != FeetState.BasePrimitive ||
	    BasedMovement.BoneName != FeetState.BaseBoneName)
	{
		FeetState.BasePrimitive = BasedMovement.MovementBase;
		FeetState.BaseBoneName = BasedMovement.BoneName;

		FVector BaseLocation;
		FQuat BaseRotation;
		MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName, BaseLocation, BaseRotation);

		ProcessFootLockBaseChange(FeetState.Left, FootLeftBone, BaseLocation, BaseRotation, CapsuleRelativeTransform);
		ProcessFootLockBaseChange(FeetState.Right, FootRightBone, BaseLocation, BaseRotation, CapsuleRelativeTransform);
	}

	FVector FinalFootLeftLocation;
	FQuat FinalFootLeftRotation;
	RefreshFootLock(FeetState.Left, FootLeftBone, UAlsConstants::FootLeftLockCurve(),
	                CapsuleRelativeTransform, DeltaTime, FinalFootLeftLocation, FinalFootLeftRotation);

	FVector FinalFootRightLocation;
	FQuat FinalFootRightRotation;
	RefreshFootLock(FeetState.Right, FootRightBone, UAlsConstants::FootRightLockCurve(),
	                CapsuleRelativeTransform, DeltaTime, FinalFootRightLocation, FinalFootRightRotation);

	const auto MeshRelativeTransform{GetSkelMeshComponent()->GetComponentTransform().Inverse()};

	if (Character->GetLocomotionMode() == FAlsLocomotionModeTags::Get().InAir ||
	    Character->GetLocomotionAction() == FAlsLocomotionActionTags::Get().Ragdolling)
	{
		ResetFootOffset(FeetState.Left, DeltaTime, FinalFootLeftLocation, FinalFootLeftRotation);
		ResetFootOffset(FeetState.Right, DeltaTime, FinalFootRightLocation, FinalFootRightRotation);

		RefreshFinalFootState(FeetState.Left, MeshRelativeTransform, FinalFootLeftLocation, FinalFootLeftRotation);
		RefreshFinalFootState(FeetState.Right, MeshRelativeTransform, FinalFootRightLocation, FinalFootRightRotation);

		RefreshPelvisOffset(DeltaTime, 0.0f, 0.0f);

		FeetState.bReinitializationRequired = false;
		return;
	}

	FVector TargetFootLeftLocationOffset;
	RefreshFootOffset(FeetState.Left, DeltaTime, TargetFootLeftLocationOffset, FinalFootLeftLocation, FinalFootLeftRotation);

	FVector TargetFootRightLocationOffset;
	RefreshFootOffset(FeetState.Right, DeltaTime, TargetFootRightLocationOffset, FinalFootRightLocation, FinalFootRightRotation);

	RefreshFinalFootState(FeetState.Left, MeshRelativeTransform, FinalFootLeftLocation, FinalFootLeftRotation);
	RefreshFinalFootState(FeetState.Right, MeshRelativeTransform, FinalFootRightLocation, FinalFootRightRotation);

	RefreshPelvisOffset(DeltaTime, TargetFootLeftLocationOffset.Z, TargetFootRightLocationOffset.Z);

	FeetState.bReinitializationRequired = false;
}

void UAlsAnimationInstance::ProcessFootLockTeleport(FAlsFootState& FootState, const FTransform& CapsuleTransform) const
{
	if (!FeetState.bReinitializationRequired)
	{
		auto LockCapsuleRelativeLocation{FootState.LockCapsuleRelativeLocation};
		LockCapsuleRelativeLocation.Z -= Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

		FootState.LockLocation = CapsuleTransform.TransformPosition(LockCapsuleRelativeLocation);
		FootState.LockRotation = CapsuleTransform.TransformRotation(FootState.LockCapsuleRelativeRotation);
	}
}

void UAlsAnimationInstance::ProcessFootLockBaseChange(FAlsFootState& FootState, const FName& FootBoneName, const FVector& BaseLocation,
                                                      const FQuat& BaseRotation, const FTransform& CapsuleRelativeTransform) const
{
	if (FeetState.bReinitializationRequired)
	{
		const auto FootTransform{GetSkelMeshComponent()->GetSocketTransform(FootBoneName)};

		FootState.LockLocation = FootTransform.GetLocation();
		FootState.LockRotation = FootTransform.GetRotation();
	}

	FootState.LockCapsuleRelativeLocation = CapsuleRelativeTransform.TransformPosition(FootState.LockLocation);
	FootState.LockCapsuleRelativeLocation.Z += Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	FootState.LockCapsuleRelativeRotation = CapsuleRelativeTransform.TransformRotation(FootState.LockRotation);

	const auto& BasedMovement{Character->GetBasedMovement()};
	if (BasedMovement.HasRelativeLocation())
	{
		const auto BaseRotationInverted{BaseRotation.Inverse()};

		FootState.LockBaseRelativeLocation = BaseRotationInverted.RotateVector(FootState.LockLocation - BaseLocation);
		FootState.LockBaseRelativeRotation = BaseRotationInverted * FootState.LockRotation;
	}
	else
	{
		FootState.LockBaseRelativeLocation = FVector::ZeroVector;
		FootState.LockBaseRelativeRotation = FQuat::Identity;
	}
}

void UAlsAnimationInstance::RefreshFootLock(FAlsFootState& FootState, const FName& FootBoneName,
                                            const FName& FootLockCurveName, const FTransform& CapsuleRelativeTransform,
                                            const float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FAnimWeight::IsRelevant(FootState.IkAmount))
	{
		return;
	}

	const auto FootTransform{GetSkelMeshComponent()->GetSocketTransform(FootBoneName)};
	auto NewFootLockAmount{GetCurveValueClamped01(FootLockCurveName)};

	if (NewFootLockAmount > FootState.LockAmount && Character->GetLocomotionMode() == FAlsLocomotionModeTags::Get().InAir)
	{
		// Smoothly disable foot locking if the character is in the air.

		static constexpr auto DecreaseSpeed{0.6f};

		NewFootLockAmount = !FeetState.bReinitializationRequired
			                    ? FMath::Max(0.0f, FootState.LockAmount - DeltaTime * DecreaseSpeed)
			                    : 0.0f;
	}

	if (Settings->Feet.bDisableFootLock || !FAnimWeight::IsRelevant(NewFootLockAmount))
	{
		if (FootState.LockAmount > 0.0f)
		{
			FootState.LockAmount = 0.0f;

			FootState.LockLocation = FVector::ZeroVector;
			FootState.LockRotation = FQuat::Identity;

			FootState.LockCapsuleRelativeLocation = FVector::ZeroVector;
			FootState.LockCapsuleRelativeRotation = FQuat::Identity;

			FootState.LockBaseRelativeLocation = FVector::ZeroVector;
			FootState.LockBaseRelativeRotation = FQuat::Identity;
		}

		FinalLocation = FootTransform.GetLocation();
		FinalRotation = FootTransform.GetRotation();
		return;
	}

	const auto& BasedMovement{Character->GetBasedMovement()};

	FVector BaseLocation;
	FQuat BaseRotation;
	MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName, BaseLocation, BaseRotation);

	const auto bNewAmountIsEqualOne{FAnimWeight::IsFullWeight(NewFootLockAmount)};
	const auto bNewAmountIsLessThanPrevious{NewFootLockAmount <= FootState.LockAmount};

	// Only update the foot lock amount if the new value is less than the current, or it equals 1. This makes it
	// so that the foot can only blend out of the locked position or lock to a new position and never blend in.

	if (bNewAmountIsEqualOne || bNewAmountIsLessThanPrevious)
	{
		FootState.LockAmount = NewFootLockAmount;

		// If the new foot lock amount equals 1 and the previous is less than 1, save the new lock location and rotation.

		if (bNewAmountIsEqualOne && !bNewAmountIsLessThanPrevious)
		{
			FootState.LockLocation = FootTransform.GetLocation();
			FootState.LockRotation = FootTransform.GetRotation();

			if (BasedMovement.HasRelativeLocation())
			{
				const auto BaseRotationInverted{BaseRotation.Inverse()};

				FootState.LockBaseRelativeLocation = BaseRotationInverted.RotateVector(FootTransform.GetLocation() - BaseLocation);
				FootState.LockBaseRelativeRotation = BaseRotationInverted * FootTransform.GetRotation();
			}
			else
			{
				FootState.LockBaseRelativeLocation = FVector::ZeroVector;
				FootState.LockBaseRelativeRotation = FQuat::Identity;
			}
		}
	}

	if (BasedMovement.HasRelativeLocation())
	{
		FootState.LockLocation = BaseLocation + BaseRotation.RotateVector(FootState.LockBaseRelativeLocation);
		FootState.LockRotation = BaseRotation * FootState.LockBaseRelativeRotation;
	}

	FootState.LockCapsuleRelativeLocation = CapsuleRelativeTransform.TransformPosition(FootState.LockLocation);
	FootState.LockCapsuleRelativeLocation.Z += Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	FootState.LockCapsuleRelativeRotation = CapsuleRelativeTransform.TransformRotation(FootState.LockRotation);

	FinalLocation = FMath::Lerp(FootTransform.GetLocation(), FootState.LockLocation, FootState.LockAmount);
	FinalRotation = FQuat::Slerp(FootTransform.GetRotation(), FootState.LockRotation, FootState.LockAmount);
}

void UAlsAnimationInstance::RefreshFootOffset(FAlsFootState& FootState, const float DeltaTime, FVector& TargetLocationOffset,
                                              FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FAnimWeight::IsRelevant(FootState.IkAmount))
	{
		FootState.bOffsetHitValid = false;

		TargetLocationOffset = FVector::ZeroVector;
		return;
	}

	const auto MeshScale{GetSkelMeshComponent()->GetComponentScale().Z};

	// Trace downward from the foot location to find the geometry. If the surface is walkable, save the impact location and normal.

	auto FootLocation{FinalLocation};
	FootLocation.Z = GetSkelMeshComponent()->GetSocketLocation(UAlsConstants::RootBone()).Z;

	FCollisionQueryParams QueryParameters{ANSI_TO_TCHAR(__FUNCTION__), true, Character};
	QueryParameters.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit,
	                                     FootLocation + FVector{0.0f, 0.0f, Settings->Feet.IkTraceDistanceUpward * MeshScale},
	                                     FootLocation - FVector{0.0f, 0.0f, Settings->Feet.IkTraceDistanceDownward * MeshScale},
	                                     UEngineTypes::ConvertToCollisionChannel(Settings->Feet.IkTraceChannel),
	                                     QueryParameters);

#if ENABLE_DRAW_DEBUG
	if (UAlsUtility::ShouldDisplayDebug(Character, UAlsConstants::TracesDisplayName()))
	{
		UAlsUtility::DrawDebugLineTraceSingle(GetWorld(), Hit.TraceStart, Hit.TraceEnd,
		                                      Character->GetCharacterMovement()->IsWalkable(Hit),
		                                      Hit, {0.0f, 0.25f, 1.0f}, {0.0f, 0.75f, 1.0f});
	}
#endif

	if (Hit.bBlockingHit)
	{
		FootState.OffsetHitLocation = Hit.ImpactPoint;
		FootState.OffsetHitNormal = Hit.ImpactNormal;
		FootState.OffsetHitComponent = Hit.Component;
		FootState.OffsetHitPhysicalMaterial = Hit.PhysMaterial;
	}

	FootState.bOffsetHitValid = Hit.bBlockingHit;

	FQuat TargetRotationOffset;

	if (Character->GetCharacterMovement()->IsWalkable(Hit))
	{
		const auto ScaledFootHeight{Settings->Feet.FootHeight * MeshScale};

		// Find the difference in location from the impact location and the expected (flat) floor location. These values
		// are offset by the impact normal multiplied by the foot height to get better behavior on angled surfaces.

		TargetLocationOffset = Hit.ImpactPoint +
		                       Hit.ImpactNormal * ScaledFootHeight -
		                       FootLocation;

		TargetLocationOffset.Z -= ScaledFootHeight;

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

	if (!FeetState.bReinitializationRequired)
	{
		static constexpr auto LocationInterpolationStiffness{20.0f};
		static constexpr auto LocationInterpolationCriticalDampingFactor{4.0f};
		static constexpr auto LocationInterpolationMass{4.0f};

		FootState.OffsetLocation = UAlsMath::InterpolateVectorSpringStable(FootState.OffsetLocation, TargetLocationOffset,
		                                                                   FootState.OffsetSpringState, LocationInterpolationStiffness,
		                                                                   LocationInterpolationCriticalDampingFactor,
		                                                                   DeltaTime, LocationInterpolationMass);

		static constexpr auto RotationInterpolationSpeed{30.0f};

		FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, TargetRotationOffset,
		                                            DeltaTime, RotationInterpolationSpeed);
	}
	else
	{
		FootState.OffsetSpringState.Reset();

		FootState.OffsetLocation = TargetLocationOffset;
		FootState.OffsetRotation = TargetRotationOffset;
	}

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;
}

void UAlsAnimationInstance::ResetFootOffset(FAlsFootState& FootState, const float DeltaTime,
                                            FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FeetState.bReinitializationRequired)
	{
		static constexpr auto LocationInterpolationStiffness{20.0f};
		static constexpr auto LocationInterpolationCriticalDampingFactor{4.0f};
		static constexpr auto LocationInterpolationMass{4.0f};

		FootState.OffsetLocation = UAlsMath::InterpolateVectorSpringStable(FootState.OffsetLocation, FVector::ZeroVector,
		                                                                   FootState.OffsetSpringState, LocationInterpolationStiffness,
		                                                                   LocationInterpolationCriticalDampingFactor,
		                                                                   DeltaTime, LocationInterpolationMass);

		static constexpr auto RotationInterpolationSpeed{15.0f};

		FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, FQuat::Identity, DeltaTime, RotationInterpolationSpeed);
	}
	else
	{
		FootState.OffsetSpringState.Reset();

		FootState.OffsetLocation = FVector::ZeroVector;
		FootState.OffsetRotation = FQuat::Identity;
	}

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;

	FootState.bOffsetHitValid = false;
}

void UAlsAnimationInstance::RefreshFinalFootState(FAlsFootState& FootState, const FTransform& MeshRelativeTransform,
                                                  const FVector& FinalLocation, const FQuat& FinalRotation)
{
	FootState.FinalMeshRelativeLocation = MeshRelativeTransform.TransformPosition(FinalLocation);
	FootState.FinalMeshRelativeRotation = MeshRelativeTransform.TransformRotation(FinalRotation);
}

void UAlsAnimationInstance::RefreshPelvisOffset(const float DeltaTime, const float TargetFootLeftLocationOffsetZ,
                                                const float TargetFootRightLocationOffsetZ)
{
	// Calculate the pelvis offset amount by finding the average foot ik weight.

	FeetState.PelvisOffsetAmount = (FeetState.Left.IkAmount + FeetState.Right.IkAmount) * 0.5f;

	// Set the new offset to be the lowest foot offset.

	const auto TargetPelvisOffsetZ{
		FMath::Min(TargetFootLeftLocationOffsetZ, TargetFootRightLocationOffsetZ) / GetSkelMeshComponent()->GetComponentScale().Z
	};

	// Interpolate current offset to the new target value.

	if (!FeetState.bReinitializationRequired)
	{
		static constexpr auto InterpolationStiffness{10.0f};
		static constexpr auto InterpolationCriticalDampingFactor{2.0f};
		static constexpr auto InterpolationMass{10.0f};

		FeetState.PelvisOffsetZ = UAlsMath::InterpolateFloatSpringStable(FeetState.PelvisOffsetZ, TargetPelvisOffsetZ,
		                                                                 FeetState.PelvisSpringState, InterpolationStiffness,
		                                                                 InterpolationCriticalDampingFactor, DeltaTime, InterpolationMass);
	}
	else
	{
		FeetState.PelvisSpringState.Reset();

		FeetState.PelvisOffsetZ = TargetPelvisOffsetZ;
	}
}

UAnimSequenceBase* UAlsAnimationInstance::SelectDynamicTransitionForLeftFoot() const
{
	return Stance.IsCrouching()
		       ? Settings->DynamicTransition.CrouchingTransitionLeftAnimation
		       : Settings->DynamicTransition.StandingTransitionLeftAnimation;
}

UAnimSequenceBase* UAlsAnimationInstance::SelectDynamicTransitionForRightFoot() const
{
	return Stance.IsCrouching()
		       ? Settings->DynamicTransition.CrouchingTransitionRightAnimation
		       : Settings->DynamicTransition.StandingTransitionRightAnimation;
}

void UAlsAnimationInstance::PlayTransition(UAnimSequenceBase* Animation, const float BlendInTime, const float BlendOutTime,
                                           const float PlayRate, const float StartTime)
{
	PlaySlotAnimationAsDynamicMontage(Animation, UAlsConstants::TransitionSlot(), BlendInTime, BlendOutTime, PlayRate, 1, 0.0f, StartTime);
}

void UAlsAnimationInstance::PlayTransitionFromStandingIdleOnly(UAnimSequenceBase* Animation, const float BlendInTime,
                                                               const float BlendOutTime, const float PlayRate, const float StartTime)
{
	if (!Character->GetLocomotionState().bMoving && Stance.IsStanding())
	{
		PlayTransition(Animation, BlendInTime, BlendOutTime, PlayRate, StartTime);
	}
}

void UAlsAnimationInstance::StopTransitionAndTurnInPlaceAnimations(const float BlendOutTime)
{
	StopSlotAnimation(BlendOutTime, UAlsConstants::TransitionSlot());
	StopSlotAnimation(BlendOutTime, UAlsConstants::TurnInPlaceStandingSlot());
	StopSlotAnimation(BlendOutTime, UAlsConstants::TurnInPlaceCrouchingSlot());
}

void UAlsAnimationInstance::RefreshTransitions()
{
	// The allow transitions curve is modified within certain states, so that allow transition will be true while in those states.

	TransitionsState.bAllowTransitions = FAnimWeight::IsFullWeight(GetCurveValue(UAlsConstants::AllowTransitionsCurve()));

	// Dynamic transitions.

	if (!bAnimationCurvesRelevant || !TransitionsState.bAllowTransitions || !TransitionsState.bAllowDynamicTransition ||
	    Character->GetLocomotionState().bMoving || Character->GetLocomotionMode() != FAlsLocomotionModeTags::Get().Grounded)
	{
		return;
	}

	// Check each foot to see if the location difference between the foot look and its desired / target location
	// (determined via a ik bone) exceeds a threshold. If it does, play an additive transition animation on that
	// foot. The currently set transition plays the second half of a 2 foot transition animation, so that only
	// a single foot moves. The separate ik bone allows the system to know its desired location when locked.

	const auto SkeletalMesh{GetSkelMeshComponent()};

	const auto FootIkDistanceThresholdSquared{
		FMath::Square(Settings->DynamicTransition.FootIkDistanceThreshold * GetSkelMeshComponent()->GetComponentScale().Z)
	};

	static constexpr auto BlendTime{0.2f};
	static constexpr auto PlayRate{1.5f};
	static constexpr auto StartTime{0.8f};
	static constexpr auto AllowanceDelay{0.1f};

	if (FVector::DistSquared(SkeletalMesh->GetSocketLocation(Settings->General.bUseFootIkBones
		                                                         ? UAlsConstants::FootLeftIkBone()
		                                                         : UAlsConstants::FootLeftIkVirtualBone()),
	                         FeetState.Left.LockLocation) > FootIkDistanceThresholdSquared)
	{
		PlayDynamicTransition(SelectDynamicTransitionForRightFoot(), BlendTime, BlendTime, PlayRate, StartTime, AllowanceDelay);
		return;
	}

	if (FVector::DistSquared(SkeletalMesh->GetSocketLocation(Settings->General.bUseFootIkBones
		                                                         ? UAlsConstants::FootRightIkBone()
		                                                         : UAlsConstants::FootRightIkVirtualBone()),
	                         FeetState.Right.LockLocation) > FootIkDistanceThresholdSquared)
	{
		PlayDynamicTransition(SelectDynamicTransitionForLeftFoot(), BlendTime, BlendTime, PlayRate, StartTime, AllowanceDelay);
	}
}

void UAlsAnimationInstance::PlayDynamicTransition(UAnimSequenceBase* Animation, const float BlendInTime, const float BlendOutTime,
                                                  const float PlayRate, const float StartTime, const float AllowanceDelay)
{
	if (TransitionsState.bAllowDynamicTransition)
	{
		TransitionsState.bAllowDynamicTransition = false;

		GetWorld()->GetTimerManager().SetTimer(TransitionsState.DynamicTransitionAllowanceTimer,
		                                       FTimerDelegate::CreateWeakLambda(
			                                       this, [&bAllowDynamicTransition = TransitionsState.bAllowDynamicTransition]
			                                       {
				                                       bAllowDynamicTransition = true;
			                                       }), AllowanceDelay, false);

		PlayTransition(Animation, BlendInTime, BlendOutTime, PlayRate, StartTime);
	}
}

bool UAlsAnimationInstance::IsRotateInPlaceAllowed()
{
	return RotationMode.IsAiming() || ViewMode == EAlsViewMode::FirstPerson;
}

void UAlsAnimationInstance::RefreshRotateInPlace(const float DeltaTime)
{
	static constexpr auto PlayRateInterpolationSpeed{5.0f};

	// Rotate in place is allowed only if the character is standing still and aiming or in first-person view mode.

	if (Character->GetLocomotionState().bMoving || !IsRotateInPlaceAllowed() ||
	    Character->GetLocomotionMode() != FAlsLocomotionModeTags::Get().Grounded)
	{
		RotateInPlaceState.bRotatingLeft = false;
		RotateInPlaceState.bRotatingRight = false;

		RotateInPlaceState.PlayRate = !bPendingUpdate
			                              ? FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlace.PlayRate.X,
			                                                 DeltaTime, PlayRateInterpolationSpeed)
			                              : Settings->RotateInPlace.PlayRate.X;

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// Check if the character should rotate left or right by checking if the view yaw angle exceeds the threshold.

	RotateInPlaceState.bRotatingLeft = ViewState.YawAngle < -Settings->RotateInPlace.ViewYawAngleThreshold;
	RotateInPlaceState.bRotatingRight = ViewState.YawAngle > Settings->RotateInPlace.ViewYawAngleThreshold;

	if (!RotateInPlaceState.bRotatingLeft && !RotateInPlaceState.bRotatingRight)
	{
		RotateInPlaceState.PlayRate = !bPendingUpdate
			                              ? FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlace.PlayRate.X,
			                                                 DeltaTime, PlayRateInterpolationSpeed)
			                              : Settings->RotateInPlace.PlayRate.X;

		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// If the character should be rotating, set the play rate to scale with the view yaw speed.
	// This makes the character rotate faster when moving the camera faster.

	const auto TargetPlayRate{
		FMath::GetMappedRangeValueClamped(Settings->RotateInPlace.ReferenceViewYawSpeed,
		                                  Settings->RotateInPlace.PlayRate, Character->GetViewState().YawSpeed)
	};

	RotateInPlaceState.PlayRate = !bPendingUpdate
		                              ? FMath::FInterpTo(RotateInPlaceState.PlayRate, TargetPlayRate,
		                                                 DeltaTime, PlayRateInterpolationSpeed)
		                              : TargetPlayRate;

	// Disable the foot lock when rotating at a large angle or rotating too fast, otherwise the legs may twist in a spiral.

	static constexpr auto BlockInterpolationSpeed{5.0f};

	if (!Settings->RotateInPlace.bDisableFootLock)
	{
		RotateInPlaceState.FootLockBlockAmount =
			FMath::Abs(ViewState.YawAngle) > Settings->RotateInPlace.FootLockBlockViewYawAngleThreshold
				? 1.0f
				: Character->GetViewState().YawSpeed <= Settings->RotateInPlace.FootLockBlockViewYawSpeedThreshold
				? 0.0f
				: bPendingUpdate
				? 1.0f
				: FMath::FInterpTo(RotateInPlaceState.FootLockBlockAmount, 1.0f, DeltaTime, BlockInterpolationSpeed);
	}
}

bool UAlsAnimationInstance::IsTurnInPlaceAllowed()
{
	return RotationMode.IsLookingDirection() && ViewMode != EAlsViewMode::FirstPerson;
}

void UAlsAnimationInstance::RefreshTurnInPlace(const float DeltaTime)
{
	// Turn in place is allowed only if transitions are allowed, the character
	// standing still and looking at the camera and not in first-person mode.

	if (Character->GetLocomotionState().bMoving || !IsTurnInPlaceAllowed() ||
	    Character->GetLocomotionMode() != FAlsLocomotionModeTags::Get().Grounded)
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		TurnInPlaceState.bDisableFootLock = false;
		return;
	}

	if (!TransitionsState.bAllowTransitions)
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		return;
	}

	// Check if the view yaw speed is below the threshold and if view yaw angle is outside of the
	// threshold. If so, begin counting the activation delay time. If not, reset the activation delay time.
	// This ensures the conditions remain true for a sustained period of time before turning in place.

	if (Character->GetViewState().YawSpeed >= Settings->TurnInPlace.ViewYawSpeedThreshold ||
	    FMath::Abs(ViewState.YawAngle) <= Settings->TurnInPlace.ViewYawAngleThreshold)
	{
		TurnInPlaceState.ActivationDelay = 0.0f;
		TurnInPlaceState.bDisableFootLock = false;
		return;
	}

	TurnInPlaceState.ActivationDelay = !bPendingUpdate
		                                   ? TurnInPlaceState.ActivationDelay + DeltaTime
		                                   : 0.0f;

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

	StartTurnInPlace(Character->GetViewState().Rotation.Yaw);
}

void UAlsAnimationInstance::StartTurnInPlace(const float TargetYawAngle, const float PlayRateScale, const float StartTime,
                                             const bool bAllowRestartIfPlaying)
{
	const auto TurnAngle{FRotator::NormalizeAxis(TargetYawAngle - Character->GetLocomotionState().Rotation.Yaw)};

	// Choose settings on the turn angle and stance.

	const FAlsTurnInPlaceSettings* TurnInPlaceSettings{nullptr};
	FName TurnInPlaceSlotName;

	if (Stance.IsStanding())
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceStandingSlot();

		if (FMath::Abs(TurnAngle) < Settings->TurnInPlace.Turn180AngleThreshold)
		{
			TurnInPlaceSettings = TurnAngle <= 0.0f || TurnAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? &Settings->TurnInPlace.StandingTurn90Left
				                      : &Settings->TurnInPlace.StandingTurn90Right;
		}
		else
		{
			TurnInPlaceSettings = TurnAngle <= 0.0f || TurnAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? &Settings->TurnInPlace.StandingTurn180Left
				                      : &Settings->TurnInPlace.StandingTurn180Right;
		}
	}
	else if (Stance.IsCrouching())
	{
		TurnInPlaceSlotName = UAlsConstants::TurnInPlaceCrouchingSlot();

		if (FMath::Abs(TurnAngle) < Settings->TurnInPlace.Turn180AngleThreshold)
		{
			TurnInPlaceSettings = TurnAngle <= 0.0f || TurnAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? &Settings->TurnInPlace.CrouchingTurn90Left
				                      : &Settings->TurnInPlace.CrouchingTurn90Right;
		}
		else
		{
			TurnInPlaceSettings = TurnAngle <= 0.0f || TurnAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
				                      ? &Settings->TurnInPlace.CrouchingTurn180Left
				                      : &Settings->TurnInPlace.CrouchingTurn180Right;
		}
	}

	// If the animation is not playing or set to be overriden, play the turn animation as a dynamic montage.

	if (TurnInPlaceSettings == nullptr ||
	    // ReSharper disable once CppRedundantParentheses
	    (!bAllowRestartIfPlaying && IsPlayingSlotAnimation(TurnInPlaceSettings->Animation, TurnInPlaceSlotName)))
	{
		return;
	}

	static constexpr auto BlendTime{0.2f};

	PlaySlotAnimationAsDynamicMontage(TurnInPlaceSettings->Animation, TurnInPlaceSlotName, BlendTime, BlendTime,
	                                  TurnInPlaceSettings->PlayRate * PlayRateScale, 1, 0.0f, StartTime);

	// Scale the rotation yaw delta (gets scaled in animation graph) to compensate for play rate and turn angle (if allowed).

	TurnInPlaceState.PlayRate = TurnInPlaceSettings->PlayRate * PlayRateScale;

	if (TurnInPlaceSettings->bScalePlayRateByAnimatedTurnAngle)
	{
		TurnInPlaceState.PlayRate *= FMath::Abs(TurnAngle / TurnInPlaceSettings->AnimatedTurnAngle);
	}

	TurnInPlaceState.bDisableFootLock = Settings->TurnInPlace.bDisableFootLock;
}

void UAlsAnimationInstance::RefreshRagdolling()
{
	if (Character->GetLocomotionAction() != FAlsLocomotionActionTags::Get().Ragdolling)
	{
		return;
	}

	// Scale the flail play rate by the root speed. The faster the ragdoll moves, the faster the character will flail.

	static constexpr auto ReferenceSpeed{1000.0f};

	RagdollingState.FlailPlayRate = UAlsMath::Clamp01(
		GetSkelMeshComponent()->GetPhysicsLinearVelocity(UAlsConstants::RootBone()).Size() / ReferenceSpeed
	);
}

void UAlsAnimationInstance::StopRagdolling()
{
	// Save a snapshot of the current ragdoll pose for use in animation graph to blend out of the ragdoll.

	SnapshotPose(RagdollingState.FinalRagdollPose);
}

void UAlsAnimationInstance::FinalizeRagdolling()
{
	Character->FinalizeRagdolling();
}

float UAlsAnimationInstance::GetCurveValueClamped01(const FName& CurveName) const
{
	return UAlsMath::Clamp01(GetCurveValue(CurveName));
}
