#include "AlsAnimationInstance.h"

#include "AlsCharacter.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utility/AlsMath.h"

const FCollisionObjectQueryParams UAlsAnimationInstance::GroundPredictionObjectQueryParameters{
	ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_Destructible)
};

void UAlsAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AAlsCharacter>(GetOwningActor());
}

void UAlsAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(Character) || DeltaTime <= SMALL_NUMBER)
	{
		return;
	}

	const auto PreviousLocomotionMode{LocomotionMode};

	Stance = Character->GetStance();
	Gait = Character->GetGait();
	RotationMode = Character->GetRotationMode();
	OverlayMode = Character->GetOverlayMode();
	LocomotionMode = Character->GetLocomotionMode();
	LocomotionAction = Character->GetLocomotionAction();

	RefreshLocomotion(DeltaTime);
	RefreshLayering();
	RefreshAiming(DeltaTime);
	RefreshFeet(DeltaTime);

	if (PreviousLocomotionMode.IsRagdolling() && !LocomotionMode.IsRagdolling())
	{
		StopRagdolling();
	}

	if (LocomotionMode.IsGrounded())
	{
		if (LocomotionState.bMoving)
		{
			MovementDirection = CalculateMovementDirection();

			RefreshMovement(DeltaTime);

			FeetState.IkRootScaleAmount = CalculateFootIkRootScaleAmount();
		}

		RefreshDynamicTransitions();

		RefreshRotateInPlace();
		RefreshTurnInPlace(DeltaTime);

		return;
	}

	RefreshInAir(DeltaTime);
	RefreshRagdolling();
}

void UAlsAnimationInstance::RefreshLocomotion(const float DeltaTime)
{
	LocomotionState.GaitTypeAmount = GetCurveValue(Constants.GaitTypeCurve);
	LocomotionState.GaitWalkingAmount = UAlsMath::Clamp01(LocomotionState.GaitTypeAmount);
	LocomotionState.GaitRunningAmount = UAlsMath::Clamp01(LocomotionState.GaitTypeAmount - 1);
	LocomotionState.GaitSprintingAmount = UAlsMath::Clamp01(LocomotionState.GaitTypeAmount - 2);

	const auto* CharacterMovement{Character->GetCharacterMovement()};

	LocomotionState.ActorRotation = Character->GetActorRotation();

	LocomotionState.bHasSpeed = Character->GetLocomotionState().bHasSpeed;
	LocomotionState.Speed = Character->GetLocomotionState().Speed;
	LocomotionState.Velocity = Character->GetLocomotionState().Velocity;
	LocomotionState.VelocityYawAngle = Character->GetLocomotionState().VelocityYawAngle;

	LocomotionState.bHasInputAcceleration = Character->GetLocomotionState().bHasInputAcceleration;

	if (LocomotionState.bHasInputAcceleration && RotationMode.IsVelocityDirection())
	{
		// Get the delta between the current acceleration rotation and character rotation and map it to a range
		// from 0 to 1. This value is used in the aiming to make the character look toward the current acceleration.

		const auto InputAccelerationYawAngle{
			FRotator::NormalizeAxis(Character->GetLocomotionState().InputAccelerationYawAngle - LocomotionState.ActorRotation.Yaw)
		};

		const auto InputAccelerationYawAmount{(InputAccelerationYawAngle / 180 + 1) / 2};

		LocomotionState.InputAccelerationYawAmount = FMath::FInterpTo(LocomotionState.InputAccelerationYawAmount,
		                                                              InputAccelerationYawAmount, DeltaTime,
		                                                              GeneralSettings.InputAccelerationYawAmountInterpolationSpeed);
	}

	LocomotionState.bMoving = Character->GetLocomotionState().bMoving;

	// The transitions allowed curve is modified within certain states, so that is transition allowed will be true while in those states.

	LocomotionState.bTransitionsAllowed = GetCurveValue(Constants.TransitionsAllowedCurve) >= 0.99f;

	// Allow movement animations if character is moving.

	if (!LocomotionState.bMoving || !LocomotionMode.IsGrounded())
	{
		return;
	}

	// Calculate the relative acceleration amount. This value represents the current amount of acceleration / deceleration
	// relative to the actor rotation. It is normalized to a range of -1 to 1 so that -1 equals the max
	// braking deceleration, and 1 equals the max acceleration of the character movement component.

	const auto Acceleration{Character->GetLocomotionState().Acceleration};

	if ((Acceleration | LocomotionState.Velocity) > 0)
	{
		const auto MaxAcceleration{CharacterMovement->GetMaxAcceleration()};

		LocomotionState.RelativeAccelerationAmount = LocomotionState.ActorRotation.UnrotateVector(
			UAlsMath::ClampMagnitude01(Acceleration / MaxAcceleration));
	}
	else
	{
		const auto MaxBrakingDeceleration{CharacterMovement->GetMaxBrakingDeceleration()};

		LocomotionState.RelativeAccelerationAmount = LocomotionState.ActorRotation.UnrotateVector(
			UAlsMath::ClampMagnitude01(Acceleration / MaxBrakingDeceleration));
	}

	// Set the rotation yaw offsets. These values influence the rotation yaw offset curve in the
	// animation graph and are used to offset the character's rotation for more natural movement.
	// The curves allow for fine control over how the offset behaves for each movement direction.

	const auto RotationYawOffset{
		FRotator::NormalizeAxis(LocomotionState.VelocityYawAngle - Character->GetAimingState().SmoothRotation.Yaw)
	};

	LocomotionState.RotationYawOffsetForward = GeneralSettings.RotationYawOffsetForwardCurve->GetFloatValue(RotationYawOffset);
	LocomotionState.RotationYawOffsetBackward = GeneralSettings.RotationYawOffsetBackwardCurve->GetFloatValue(RotationYawOffset);
	LocomotionState.RotationYawOffsetLeft = GeneralSettings.RotationYawOffsetLeftCurve->GetFloatValue(RotationYawOffset);
	LocomotionState.RotationYawOffsetRight = GeneralSettings.RotationYawOffsetRightCurve->GetFloatValue(RotationYawOffset);

	// Use the relative acceleration as the sprint relative acceleration if less than 0.5 seconds has elapsed
	// since the start of the sprint, otherwise set the sprint relative acceleration to zero.This is
	// necessary in order to apply the acceleration animation only at the beginning of the sprint.

	if (Gait.IsSprinting())
	{
		LocomotionState.SprintTime += DeltaTime;
		LocomotionState.SprintRelativeAccelerationAmount = LocomotionState.SprintTime < 0.5f
			                                                   ? LocomotionState.RelativeAccelerationAmount.X
			                                                   : 0;
	}
	else
	{
		LocomotionState.SprintTime = 0;
		LocomotionState.SprintRelativeAccelerationAmount = 0;
	}
}

void UAlsAnimationInstance::RefreshLayering()
{
	LayeringState.HeadBlendAmount = GetCurveValueClamped01(Constants.LayerHeadCurve);
	LayeringState.HeadAdditiveBlendAmount = GetCurveValueClamped01(Constants.LayerHeadAdditiveCurve);

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmLeftBlendAmount = GetCurveValueClamped01(Constants.LayerArmLeftCurve);
	LayeringState.ArmLeftAdditiveBlendAmount = GetCurveValueClamped01(Constants.LayerArmLeftAdditiveCurve);
	LayeringState.ArmLeftLocalSpaceBlendAmount = GetCurveValueClamped01(Constants.LayerArmLeftLocalSpaceCurve);
	LayeringState.ArmLeftMeshSpaceBlendAmount = 1 - FMath::FloorToInt(LayeringState.ArmLeftLocalSpaceBlendAmount);

	// The mesh space blend will always be 1 unless the local space blend is 1.

	LayeringState.ArmRightBlendAmount = GetCurveValueClamped01(Constants.LayerArmRightCurve);
	LayeringState.ArmRightAdditiveBlendAmount = GetCurveValueClamped01(Constants.LayerArmRightAdditiveCurve);
	LayeringState.ArmRightLocalSpaceBlendAmount = GetCurveValueClamped01(Constants.LayerArmRightLocalSpaceCurve);
	LayeringState.ArmRightMeshSpaceBlendAmount = 1 - FMath::FloorToInt(LayeringState.ArmRightLocalSpaceBlendAmount);

	LayeringState.HandLeftBlendAmount = GetCurveValueClamped01(Constants.LayerHandLeftCurve);
	LayeringState.HandRightBlendAmount = GetCurveValueClamped01(Constants.LayerHandRightCurve);

	LayeringState.SpineBlendAmount = GetCurveValueClamped01(Constants.LayerSpineCurve);
	LayeringState.SpineAdditiveBlendAmount = GetCurveValueClamped01(Constants.LayerSpineAdditiveCurve);

	LayeringState.PelvisBlendAmount = GetCurveValueClamped01(Constants.LayerPelvisCurve);
	LayeringState.LegsBlendAmount = GetCurveValueClamped01(Constants.LayerLegsCurve);

	LayeringState.PoseStandingBlendAmount = GetCurveValueClamped01(Constants.PoseStandCurve);
	LayeringState.PoseCrouchingBlendAmount = GetCurveValueClamped01(Constants.PoseCrouchCurve);

	// Hand ik is scaled by the arms blend.

	LayeringState.HandLeftIkAmount = LayeringState.ArmLeftBlendAmount * GetCurveValueClamped01(Constants.HandLeftIkCurve);
	LayeringState.HandRightIkAmount = LayeringState.ArmRightBlendAmount * GetCurveValueClamped01(Constants.HandRightIkCurve);
}

void UAlsAnimationInstance::RefreshAiming(const float DeltaTime)
{
	AimingState.Rotation = Character->GetAimingState().SmoothRotation;

	const auto AimingRotation{AimingState.Rotation - LocomotionState.ActorRotation};

	AimingState.YawAngle = FRotator::NormalizeAxis(AimingRotation.Yaw);
	AimingState.PitchAngle = FRotator::NormalizeAxis(AimingRotation.Pitch);

	AimingState.YawSpeed = Character->GetAimingState().YawSpeed;

	// Interpolate the aiming rotation value to achieve smooth aiming rotation changes. Interpolating
	// the rotation before calculating the angle ensures the value is not affected by changes in
	// actor rotation, allowing slow aiming rotation changes with fast actor rotation changes.

	AimingState.SmoothRotation = FMath::RInterpTo(AimingState.SmoothRotation, AimingState.Rotation, DeltaTime,
	                                              GeneralSettings.AimingSmoothRotationInterpolationSpeed);

	const auto AimingSmoothRotation{AimingState.SmoothRotation - LocomotionState.ActorRotation};

	AimingState.SmoothYawAngle = FRotator::NormalizeAxis(AimingSmoothRotation.Yaw);
	AimingState.SmoothPitchAngle = FRotator::NormalizeAxis(AimingSmoothRotation.Pitch);

	// Separate the Smooth aiming yaw angle into 3 separate values. These 3 values are used to
	// improve the blending of the aiming when rotating completely around the character. This allows
	// you to keep the aiming responsive but still smoothly blend from left to right or right to left.

	AimingState.SmoothYawAmount = (AimingState.SmoothYawAngle / 180 + 1) / 2;
	AimingState.SmoothYawLeftAmount = FMath::GetMappedRangeValueClamped({0, 180}, {0.5, 0}, FMath::Abs(AimingState.SmoothYawAngle));
	AimingState.SmoothYawRightAmount = FMath::GetMappedRangeValueClamped({0, 180}, {0.5, 1}, FMath::Abs(AimingState.SmoothYawAngle));

	if (RotationMode.IsAiming())
	{
		// Individual rotations for 3 spine bones and pelvis.

		AimingState.SpineRotation.Yaw = FMath::ClampAngle(AimingState.YawAngle / 4, -70 / 4.0f, 70 / 4.0f);
	}

	if (!RotationMode.IsVelocityDirection())
	{
		AimingState.PitchAmount = FMath::GetMappedRangeValueClamped({-90, 90}, {1, 0}, AimingState.PitchAngle);
	}
}

void UAlsAnimationInstance::RefreshFeet(const float DeltaTime)
{
	FeetState.IkLeftAmount = GetCurveValueClamped01(Constants.FootLeftIkCurve);
	FeetState.IkRightAmount = GetCurveValueClamped01(Constants.FootRightIkCurve);

	if (FeetSettings.bEnableFootLocking)
	{
		RefreshFootLock(FeetState.LockLeft, FeetState.IkLeftAmount, DeltaTime,
		                Constants.FootLeftLockVirtualBone, Constants.FootLeftLockCurve);
		RefreshFootLock(FeetState.LockRight, FeetState.IkRightAmount, DeltaTime,
		                Constants.FootRightLockVirtualBone, Constants.FootRightLockCurve);
	}

	FeetState.FootPlanted = FMath::Clamp(GetCurveValue(Constants.FootPlantedCurve), -1.0f, 1.0f);

	if (LocomotionMode.IsInAir())
	{
		ResetFootOffset(FeetState.OffsetLeft, DeltaTime);
		ResetFootOffset(FeetState.OffsetRight, DeltaTime);

		RefreshPelvisOffset(DeltaTime, FVector::ZeroVector, FVector::ZeroVector);
		return;
	}

	if (LocomotionMode.IsRagdolling())
	{
		return;
	}

	auto TargetFootLeftLocationOffset{FVector::ZeroVector};
	auto TargetFootRightLocationOffset{FVector::ZeroVector};

	RefreshFootOffset(FeetState.OffsetLeft, FeetState.IkLeftAmount, DeltaTime,
	                  Constants.FootLeftLockVirtualBone, TargetFootLeftLocationOffset);
	RefreshFootOffset(FeetState.OffsetRight, FeetState.IkRightAmount, DeltaTime,
	                  Constants.FootRightLockVirtualBone, TargetFootRightLocationOffset);

	RefreshPelvisOffset(DeltaTime, TargetFootLeftLocationOffset, TargetFootRightLocationOffset);
}

void UAlsAnimationInstance::RefreshFootLock(FAlsFootLockState& FootLockState, const float FootIkAmount, const float DeltaTime,
                                            const FName FootLockBoneName, const FName FootLockCurveName) const
{
	if (FootIkAmount <= SMALL_NUMBER)
	{
		return;
	}

	// float NewFootLockAmount;
	//
	// if (FootLockState.bUseFootLockCurve)
	// {
	// 	FootLockState.bUseFootLockCurve = FMath::Abs(GetCurveValue(Constants.RotationYawSpeedCurve)) <= KINDA_SMALL_NUMBER ||
	// 	                                  Character->GetLocalRole() != ROLE_AutonomousProxy;
	// 	NewFootLockAmount = GetCurveValue(FootLockCurveName);
	// }
	// else
	// {
	// 	FootLockState.bUseFootLockCurve = GetCurveValue(FootLockCurveName) >= 0.99;
	// 	NewFootLockAmount = 0;
	// }

	const auto NewFootLockAmount{GetCurveValue(FootLockCurveName)};

	// Only update the foot lock amount if the new value is less than the current, or it equals 1. This makes it
	// so that the foot can only blend out of the locked position or lock to a new position, and never blend in.

	if (NewFootLockAmount >= 0.99 || NewFootLockAmount < FootLockState.Amount)
	{
		FootLockState.Amount = NewFootLockAmount;
	}

	const auto OwningComponent{GetOwningComponent()};

	// If the foot lock amount equals 1, save the new lock location and rotation in component space as the target.

	if (FootLockState.Amount >= 0.99)
	{
		const auto Transform{OwningComponent->GetSocketTransform(FootLockBoneName, RTS_Component)};
		FootLockState.Location = Transform.GetLocation();
		FootLockState.Rotation = Transform.Rotator();
	}

	// If the foot lock amount is not equals 0, update the offsets to keep the foot planted in place while the capsule moves.

	if (FootLockState.Amount <= SMALL_NUMBER)
	{
		return;
	}

	FRotator RotationDifference;

	// Use the delta between the current and last updated rotation to find
	// how much the foot should be rotated to remain planted on the ground.

	if (LocomotionMode.IsGrounded())
	{
		RotationDifference = LocomotionState.ActorRotation - Character->GetCharacterMovement()->GetLastUpdateRotation();
		RotationDifference.Normalize();
	}
	else
	{
		RotationDifference = FRotator::ZeroRotator;
	}

	// Get the distance traveled between frames relative to the mesh rotation
	// to find how much the foot should be offset to remain planted on the ground.

	const auto LocationDifference{OwningComponent->GetComponentRotation().UnrotateVector(LocomotionState.Velocity * DeltaTime)};

	// Subtract the location difference from the current local location and rotate
	// it by the rotation difference to keep the foot planted in component space.

	FootLockState.Location = (FootLockState.Location - LocationDifference).RotateAngleAxis(RotationDifference.Yaw, FVector::DownVector);

	// Subtract the rotation difference from the current rotation to get the new local rotation.

	FootLockState.Rotation = {FootLockState.Rotation - RotationDifference};
	FootLockState.Rotation.Normalize();
}

void UAlsAnimationInstance::RefreshFootOffset(FAlsFootOffsetState& FootOffsetState, const float FootIkAmount, const float DeltaTime,
                                              const FName FootLockBoneName, FVector& TargetLocationOffset) const
{
	if (FootIkAmount <= SMALL_NUMBER)
	{
		FootOffsetState.Location = FVector::ZeroVector;
		FootOffsetState.Rotation = FRotator::ZeroRotator;
		return;
	}

	const auto OwningComponent{GetOwningComponent()};

	// Trace downward from the foot location to find the geometry. If the surface is walkable, save the impact location and normal.

	auto FootLockBoneLocation{OwningComponent->GetSocketLocation(FootLockBoneName)};
	FootLockBoneLocation.Z = OwningComponent->GetSocketLocation(Constants.RootBone).Z;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit,
	                                     FootLockBoneLocation + FVector{0, 0, FeetSettings.IkTraceDistanceUpward},
	                                     FootLockBoneLocation - FVector{0, 0, FeetSettings.IkTraceDistanceDownward},
	                                     ECC_Visibility, {TEXT("AlsAnimationInstance::RefreshFootOffset"), true, Character});

	auto TargetRotationOffset{FRotator::ZeroRotator};

	if (Character->GetCharacterMovement()->IsWalkable(Hit))
	{
		// Find the difference in location from the impact location and the expected (flat) floor location. These values
		// are offset by the impact normal multiplied by the foot height to get better behavior on angled surfaces.

		TargetLocationOffset = Hit.ImpactPoint +
		                       Hit.ImpactNormal * FeetSettings.FootHeight -
		                       FootLockBoneLocation;

		TargetLocationOffset.Z -= FeetSettings.FootHeight;

		// Calculate the rotation offset.

		TargetRotationOffset.Pitch = -UAlsMath::DirectionToAngle({Hit.ImpactNormal.Z, Hit.ImpactNormal.X});
		TargetRotationOffset.Roll = UAlsMath::DirectionToAngle({Hit.ImpactNormal.Z, Hit.ImpactNormal.Y});
	}

	const auto CorrectedLocationOffset{TargetLocationOffset + FVector{0, 0, FeetSettings.IkVerticalCorrection}};

	// Interpolate the current location offset to the new target value. Interpolate at
	// different speeds based on whether the new target is above or below the current one.

	FootOffsetState.Location = FMath::VInterpTo(FootOffsetState.Location, CorrectedLocationOffset, DeltaTime,
	                                            FootOffsetState.Location.Z > CorrectedLocationOffset.Z ? 30 : 15);

	// Interpolate the current rotation offset to the new target value.

	FootOffsetState.Rotation = FMath::RInterpTo(FootOffsetState.Rotation, TargetRotationOffset, DeltaTime, 30);
}

void UAlsAnimationInstance::ResetFootOffset(FAlsFootOffsetState& FootOffsetState, const float DeltaTime)
{
	FootOffsetState.Location = FMath::VInterpTo(FootOffsetState.Location, FVector::ZeroVector, DeltaTime, 15);
	FootOffsetState.Rotation = FMath::RInterpTo(FootOffsetState.Rotation, FRotator::ZeroRotator, DeltaTime, 15);
}

void UAlsAnimationInstance::RefreshPelvisOffset(const float DeltaTime, const FVector& TargetFootLeftLocationOffset,
                                                const FVector& TargetFootRightLocationOffset)
{
	// Calculate the pelvis offset amount by finding the average foot ik weight. If the amount is 0, clear the offset.

	FeetState.PelvisOffsetAmount = (FeetState.IkLeftAmount + FeetState.IkRightAmount) / 2;

	if (FeetState.PelvisOffsetAmount <= SMALL_NUMBER)
	{
		FeetState.PelvisLocationOffset = FVector::ZeroVector;
		return;
	}

	// Set the new location offset to be the lowest foot offset.

	const auto TargetOffset{
		TargetFootLeftLocationOffset.Z < TargetFootRightLocationOffset.Z
			? TargetFootLeftLocationOffset
			: TargetFootRightLocationOffset
	};

	// Interpolate the current location offset to the new target value. Interpolate at
	// different speeds based on whether the new target is above or below the current one.

	FeetState.PelvisLocationOffset = FMath::VInterpTo(FeetState.PelvisLocationOffset, TargetOffset, DeltaTime,
	                                                  TargetOffset.Z > FeetState.PelvisLocationOffset.Z ? 10 : 15);
}

float UAlsAnimationInstance::CalculateFootIkRootScaleAmount() const
{
	// Calculate the foot ik root scale amount. This value is used to scale the foot ik root bone to make the foot ik bones
	// cover more distance on the diagonal blends. Without scaling, the feet would not move far enough on the diagonal
	// direction due to the linear translational blending of the ik bones. The curve is used to easily map the value.

	return FeetSettings.IkRootScaleAmountCurve->GetFloatValue(
		MovementState.VelocityBlend.ForwardAmount + MovementState.VelocityBlend.BackwardAmount);
}

EAlsMovementDirection UAlsAnimationInstance::CalculateMovementDirection() const
{
	// Calculate the movement direction. This value represents the direction the character is moving relative to the camera
	// during the looking direction / aiming modes, and is used in the cycle blending to blend to the appropriate directional states.

	if (Gait.IsSprinting() || RotationMode.IsVelocityDirection())
	{
		return EAlsMovementDirection::Forward;
	}

	return UAlsMath::CalculateMovementDirection(
		FRotator::NormalizeAxis(LocomotionState.VelocityYawAngle - AimingState.Rotation.Yaw),
		70, 5);
}

void UAlsAnimationInstance::RefreshMovement(const float DeltaTime)
{
	RefreshVelocityBlend(DeltaTime);

	MovementState.StrideBlendAmount = CalculateStrideBlendAmount();
	MovementState.WalkRunBlendAmount = CalculateWalkRunBlendAmount();
	MovementState.StandingPlayRate = CalculateStandingPlayRate();
	MovementState.CrouchingPlayRate = CalculateCrouchingPlayRate();

	// Interpolate the lean amount.

	LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, LocomotionState.RelativeAccelerationAmount.Y, DeltaTime,
	                                         GeneralSettings.LeanInterpolationSpeed);

	LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, LocomotionState.RelativeAccelerationAmount.X, DeltaTime,
	                                           GeneralSettings.LeanInterpolationSpeed);
}

void UAlsAnimationInstance::RefreshVelocityBlend(const float DeltaTime)
{
	// Calculate and interpolate the velocity blend. This value represents the velocity amount of the
	// actor in each direction (normalized so that diagonals equal 0.5 for each direction), and is
	// used in a blend multi node to produce better directional blending than a standard blend space.

	const auto RelativeVelocityDirection{LocomotionState.ActorRotation.UnrotateVector(LocomotionState.Velocity.GetSafeNormal())};

	const auto RelativeDirection = RelativeVelocityDirection /
	                               (FMath::Abs(RelativeVelocityDirection.X) +
	                                FMath::Abs(RelativeVelocityDirection.Y) +
	                                FMath::Abs(RelativeVelocityDirection.Z));

	MovementState.VelocityBlend.ForwardAmount = FMath::FInterpTo(MovementState.VelocityBlend.ForwardAmount,
	                                                             UAlsMath::Clamp01(RelativeDirection.X), DeltaTime,
	                                                             MovementSettings.VelocityBlendInterpolationSpeed);

	MovementState.VelocityBlend.BackwardAmount = FMath::FInterpTo(MovementState.VelocityBlend.BackwardAmount,
	                                                              FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f)), DeltaTime,
	                                                              MovementSettings.VelocityBlendInterpolationSpeed);

	MovementState.VelocityBlend.LeftAmount = FMath::FInterpTo(MovementState.VelocityBlend.LeftAmount,
	                                                          FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f)), DeltaTime,
	                                                          MovementSettings.VelocityBlendInterpolationSpeed);

	MovementState.VelocityBlend.RightAmount = FMath::FInterpTo(MovementState.VelocityBlend.RightAmount,
	                                                           UAlsMath::Clamp01(RelativeDirection.Y), DeltaTime,
	                                                           MovementSettings.VelocityBlendInterpolationSpeed);
}

float UAlsAnimationInstance::CalculateStrideBlendAmount() const
{
	// Calculate the stride blend. This value is used within the blend spaces to scale the stride (distance feet travel) so that the
	// character can walk or run at different movement speeds. It also allows the walk or run gait animations to blend independently
	// while still matching the animation speed to the movement speed, preventing the character from needing to play a half walk + half
	// run blend. The curves are used to map the stride amount to the speed for maximum control.

	const auto Speed{LocomotionState.Speed / GetOwningComponent()->GetComponentScale().Z};

	const auto StandingStrideBlend{
		FMath::Lerp(MovementSettings.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
		            MovementSettings.StrideBlendAmountRunCurve->GetFloatValue(Speed),
		            LocomotionState.GaitRunningAmount)
	};

	// Crouching stride blends.

	return FMath::Lerp(StandingStrideBlend,
	                   MovementSettings.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
	                   LayeringState.PoseCrouchingBlendAmount);
}

float UAlsAnimationInstance::CalculateWalkRunBlendAmount() const
{
	// Calculate the walk run blend. This value is used within the blend spaces to blend between walking and running.

	return Gait.IsWalking() ? 0 : 1;
}

float UAlsAnimationInstance::CalculateStandingPlayRate() const
{
	// Calculate the standing play rate by dividing the character's speed by the animated speed for each gait. The interpolation are
	// determined by the gait type curve that exists on every locomotion cycle so that the play rate is always in sync with the currently
	// blended animation. The value is also divided by the stride blend and the mesh scale so that the play rate increases as the stride
	// or scale gets smaller.

	const auto WalkRunSpeedAmount{
		FMath::Lerp(LocomotionState.Speed / MovementSettings.AnimatedWalkSpeed,
		            LocomotionState.Speed / MovementSettings.AnimatedRunSpeed,
		            LocomotionState.GaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount{
		FMath::Lerp(WalkRunSpeedAmount,
		            LocomotionState.Speed / MovementSettings.AnimatedSprintSpeed,
		            LocomotionState.GaitSprintingAmount)
	};

	return FMath::Clamp(WalkRunSprintSpeedAmount / MovementState.StrideBlendAmount / GetOwningComponent()->GetComponentScale().Z, 0.0f,
	                    3.0f);
}

float UAlsAnimationInstance::CalculateCrouchingPlayRate() const
{
	// Calculate the crouching play rate by dividing the character's speed by the animated speed. This value needs
	// to be separate from the standing play rate to improve the blend from crouching to standing while in motion.

	return FMath::Clamp(LocomotionState.Speed / MovementSettings.AnimatedCrouchSpeed /
	                    MovementState.StrideBlendAmount / GetOwningComponent()->GetComponentScale().Z,
	                    0.0f, 2.0f);
}

void UAlsAnimationInstance::ActivatePivot()
{
	MovementState.bPivotActive = LocomotionState.Speed < MovementSettings.PivotActivationSpeedThreshold;

	if (MovementState.bPivotActive)
	{
		GetWorld()->GetTimerManager().SetTimer(PivotResetTimer, this, &ThisClass::OnPivotResetTimerEnded, 0.1, false);
	}
}

void UAlsAnimationInstance::OnPivotResetTimerEnded()
{
	MovementState.bPivotActive = false;
}

void UAlsAnimationInstance::PlayTransition(UAnimSequenceBase* Animation, const float BlendInTime, const float BlendOutTime,
                                           const float PlayRate, const float StartTime)
{
	PlaySlotAnimationAsDynamicMontage(Animation, Constants.TransitionSlot, BlendInTime, BlendOutTime, PlayRate, 1, 0, StartTime);
}

void UAlsAnimationInstance::PlayTransitionFromStandingIdleOnly(UAnimSequenceBase* Animation, const float BlendInTime,
                                                               const float BlendOutTime,
                                                               const float PlayRate, const float StartTime)
{
	if (!LocomotionState.bMoving && Stance.IsStanding())
	{
		PlayTransition(Animation, BlendInTime, BlendOutTime, PlayRate, StartTime);
	}
}

void UAlsAnimationInstance::StopTransitionAndTurnInPlaceSlotAnimations(const float BlendOutTime)
{
	StopSlotAnimation(BlendOutTime, Constants.TransitionSlot);
	StopSlotAnimation(BlendOutTime, Constants.TurnInPlaceSlot);
}

void UAlsAnimationInstance::RefreshDynamicTransitions()
{
	if (LocomotionState.bMoving || !LocomotionState.bTransitionsAllowed)
	{
		return;
	}

	// Check each foot to see if the location difference between the foot look bone and its desired / target location (determined
	// via a virtual bone) exceeds a threshold. If it does, play an additive transition animation on that foot. The currently
	// set transition plays the second half of a 2 foot transition animation, so that only a single foot moves. Because only
	// the foot look bone can be locked, the separate virtual bone allows the system to know its desired location when locked.

	const auto OwningComponent{GetOwningComponent()};

	auto FootLockBoneLocation{
		OwningComponent->GetSocketTransform(Constants.FootLeftLockVirtualBone, RTS_Component).GetLocation()
	};

	auto FootBoneLocation{
		OwningComponent->GetSocketTransform(Constants.FootLeftVirtualBone, RTS_Component).GetLocation()
	};

	if (FVector::DistSquared(FootBoneLocation, FootLockBoneLocation) > FMath::Square(DynamicTransitionSettings.FootIkDistanceThreshold))
	{
		PlayDynamicTransition(DynamicTransitionSettings.TransitionRightAnimation, 0.2, 0.2, 1.5, 0.8, 0.1);
		return;
	}

	FootLockBoneLocation = OwningComponent->GetSocketTransform(Constants.FootRightLockVirtualBone, RTS_Component).GetLocation();
	FootBoneLocation = OwningComponent->GetSocketTransform(Constants.FootRightVirtualBone, RTS_Component).GetLocation();

	if (FVector::DistSquared(FootBoneLocation, FootLockBoneLocation) > FMath::Square(DynamicTransitionSettings.FootIkDistanceThreshold))
	{
		PlayDynamicTransition(DynamicTransitionSettings.TransitionLeftAnimation, 0.2, 0.2, 1.5, 0.8, 0.1);
	}
}

void UAlsAnimationInstance::PlayDynamicTransition(UAnimSequenceBase* Animation, const float BlendInTime, const float BlendOutTime,
                                                  const float PlayRate, const float StartTime, const float AllowanceDelayTime)
{
	if (bDynamicTransitionsAllowed)
	{
		bDynamicTransitionsAllowed = false;

		GetWorld()->GetTimerManager().SetTimer(DynamicTransitionsAllowanceTimer, this,
		                                       &UAlsAnimationInstance::OnDynamicTransitionAllowanceTimerEnded, AllowanceDelayTime, false);

		PlayTransition(Animation, BlendInTime, BlendOutTime, PlayRate, StartTime);
	}
}

void UAlsAnimationInstance::OnDynamicTransitionAllowanceTimerEnded()
{
	bDynamicTransitionsAllowed = true;
}

void UAlsAnimationInstance::RefreshRotateInPlace()
{
	// Rotate in place is allowed only if the character is standing still and aiming.

	if (LocomotionState.bMoving || !RotationMode.IsAiming())
	{
		RotateInPlaceState.bRotatingLeft = false;
		RotateInPlaceState.bRotatingRight = false;
		return;
	}

	// Check if the character should rotate left or right by checking if the aiming yaw angle exceeds the threshold.

	const auto bRotatingLeftPrevious{RotateInPlaceState.bRotatingLeft};
	const auto bRotatingRightPrevious{RotateInPlaceState.bRotatingRight};

	RotateInPlaceState.bRotatingLeft = AimingState.YawAngle < -RotateInPlaceSettings.AimingYawAngleThreshold;
	RotateInPlaceState.bRotatingRight = AimingState.YawAngle > RotateInPlaceSettings.AimingYawAngleThreshold;

	if (!bRotatingLeftPrevious && RotateInPlaceState.bRotatingLeft ||
	    !bRotatingRightPrevious && RotateInPlaceState.bRotatingRight)
	{
		RotateInPlaceState.InitialPlayRate = FMath::Lerp(RotateInPlaceSettings.PlayRate.X, RotateInPlaceSettings.PlayRate.Y,
		                                                 UAlsMath::Clamp01(FMath::Abs(AimingState.YawAngle / 180)));
	}

	if (RotateInPlaceState.bRotatingLeft || RotateInPlaceState.bRotatingRight)
	{
		// If rotating too fast, then disable the foot lock, or else the legs begin to twist into a spiral.

		RotateInPlaceState.bDisableFootLock = AimingState.YawSpeed > RotateInPlaceSettings.MaxFootLockAimingYawSpeed;

		// If the character should be rotating, set the play rate to scale with the aiming yaw speed.
		// This makes the character rotate faster when moving the camera faster.

		const auto PlayRateFromYawSpeed{
			FMath::GetMappedRangeValueClamped(RotateInPlaceSettings.ReferenceAimingYawSpeed,
			                                  RotateInPlaceSettings.PlayRate,
			                                  AimingState.YawSpeed)
		};

		if (RotateInPlaceState.InitialPlayRate > PlayRateFromYawSpeed)
		{
			RotateInPlaceState.PlayRate = RotateInPlaceState.InitialPlayRate;
		}
		else
		{
			RotateInPlaceState.InitialPlayRate = 0;
			RotateInPlaceState.PlayRate = PlayRateFromYawSpeed;
		}
	}
}

void UAlsAnimationInstance::RefreshTurnInPlace(const float DeltaTime)
{
	// Turn in place is allowed only if transitions are allowed, the character standing still and looking at the camera.

	if (LocomotionState.bMoving || !LocomotionState.bTransitionsAllowed || !RotationMode.IsLookingDirection())
	{
		TurnInPlaceState.ActivationDelayTime = 0;
		return;
	}

	// Check if the aiming yaw speed is below the threshold, and if aiming yaw angle is outside of the
	// threshold. If so, begin counting the activation delay time. If not, reset the activation delay time.
	// This ensures the conditions remain true for a sustained period of time before turning in place.

	if (AimingState.YawSpeed >= TurnInPlaceSettings.AimingYawSpeedThreshold ||
	    FMath::Abs(AimingState.YawAngle) <= TurnInPlaceSettings.AimingYawAngleThreshold)
	{
		TurnInPlaceState.ActivationDelayTime = 0;
		return;
	}

	TurnInPlaceState.ActivationDelayTime += DeltaTime;

	const auto ActivationDelay{
		FMath::GetMappedRangeValueClamped({TurnInPlaceSettings.AimingYawAngleThreshold, 180},
		                                  TurnInPlaceSettings.AimingYawAngleToActivationDelay,
		                                  FMath::Abs(AimingState.YawAngle))
	};

	// Check if the activation delay time exceeds the set delay (mapped to the aiming yaw angle). If so, start a turn in place.

	if (TurnInPlaceState.ActivationDelayTime <= ActivationDelay)
	{
		return;
	}

	StartTurnInPlace(AimingState.Rotation.Yaw);
}

void UAlsAnimationInstance::StartTurnInPlace(const float TargetYawAngle, const float PlayRateScale, const float StartTime,
                                             const bool bAllowRestartIfPlaying)
{
	const auto TurnAngle{FRotator::NormalizeAxis(TargetYawAngle - LocomotionState.ActorRotation.Yaw)};

	// Choose settings on the turn angle and stance.

	FAlsTurnInPlaceSettings Settings;
	if (Stance.IsStanding())
	{
		if (FMath::Abs(TurnAngle) < TurnInPlaceSettings.Turn180AngleThreshold)
		{
			Settings = TurnAngle < 0
				           ? TurnInPlaceSettings.StandingTurn90Left
				           : TurnInPlaceSettings.StandingTurn90Right;
		}
		else
		{
			Settings = TurnAngle < 0
				           ? TurnInPlaceSettings.StandingTurn180Left
				           : TurnInPlaceSettings.StandingTurn180Right;
		}
	}
	else
	{
		if (FMath::Abs(TurnAngle) < TurnInPlaceSettings.Turn180AngleThreshold)
		{
			Settings = TurnAngle < 0
				           ? TurnInPlaceSettings.CrouchingTurn90Left
				           : TurnInPlaceSettings.CrouchingTurn90Right;
		}
		else
		{
			Settings = TurnAngle < 0
				           ? TurnInPlaceSettings.CrouchingTurn180Left
				           : TurnInPlaceSettings.CrouchingTurn180Right;
		}
	}

	// If the animation is not playing or set to be overriden, play the turn animation as a dynamic montage.

	if (!bAllowRestartIfPlaying && IsPlayingSlotAnimation(Settings.Animation, Constants.TurnInPlaceSlot))
	{
		return;
	}

	PlaySlotAnimationAsDynamicMontage(Settings.Animation, Constants.TurnInPlaceSlot, 0.2, 0.2, Settings.PlayRate * PlayRateScale, 1,
	                                  0, StartTime);

	// Scale the rotation yaw delta (gets scaled in animation graph) to compensate for play rate and turn angle (if allowed).

	if (Settings.bScalePlayRateByAnimatedTurnAngle)
	{
		TurnInPlaceState.PlayRate = Settings.PlayRate * PlayRateScale * TurnAngle / Settings.AnimatedTurnAngle;
	}
	else
	{
		TurnInPlaceState.PlayRate = Settings.PlayRate * PlayRateScale;
	}
}

void UAlsAnimationInstance::Jump()
{
	InAirState.bJumped = true;
	InAirState.JumpPlayRate = UAlsMath::LerpClamped(1.2, 1.5, LocomotionState.Speed / 600);

	GetWorld()->GetTimerManager().SetTimer(JumpResetTimer, this, &ThisClass::OnJumpResetTimerEnded, 0.1, false);
}

void UAlsAnimationInstance::OnJumpResetTimerEnded()
{
	InAirState.bJumped = false;
}

void UAlsAnimationInstance::RefreshInAir(const float DeltaTime)
{
	if (!LocomotionMode.IsInAir())
	{
		return;
	}

	// Update the vertical velocity. Setting this value only while in the air allows you to use it within the
	// animation graph for the landing strength. If not, the vertical velocity would return to 0 on landing.

	InAirState.VerticalVelocity = LocomotionState.Velocity.Z;

	// Set the ground prediction amount.

	InAirState.GroundPredictionAmount = CalculateGroundPredictionAmount();

	// Interpolate the lean amount.

	const auto TargetLeanAmount{CalculateInAirLeanAmount()};

	LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, TargetLeanAmount.RightAmount,
	                                         DeltaTime, GeneralSettings.LeanInterpolationSpeed);
	LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, TargetLeanAmount.ForwardAmount,
	                                           DeltaTime, GeneralSettings.LeanInterpolationSpeed);
}

float UAlsAnimationInstance::CalculateGroundPredictionAmount() const
{
	// Calculate the ground prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward, and getting the "time" (range from 0 to 1, 1 being maximum, 0 being about to ground) till impact.
	// The ground prediction amount curve is used to control how the time affects the final amount for a smooth blend.

	if (InAirState.VerticalVelocity >= -200)
	{
		return 0;
	}

	const auto* CharacterCapsule{Character->GetCapsuleComponent()};
	const auto CapsuleLocation{CharacterCapsule->GetComponentLocation()};

	auto VelocityDirection{LocomotionState.Velocity};
	VelocityDirection.Z = FMath::Clamp(InAirState.VerticalVelocity, -4000.0f, -200.0f);
	VelocityDirection.Normalize();

	const auto TraceVector{VelocityDirection * FMath::GetMappedRangeValueClamped({0, -4000}, {50, 2000}, InAirState.VerticalVelocity)};

	FHitResult Hit;
	GetWorld()->SweepSingleByObjectType(Hit, CapsuleLocation, CapsuleLocation + TraceVector, FQuat::Identity,
	                                    GroundPredictionObjectQueryParameters,
	                                    FCollisionShape::MakeCapsule(CharacterCapsule->GetScaledCapsuleRadius(),
	                                                                 CharacterCapsule->GetScaledCapsuleHalfHeight()),
	                                    {TEXT("AlsAnimationInstance::CalculateGroundPredictionAmount"), false, Character});

	if (!Character->GetCharacterMovement()->IsWalkable(Hit))
	{
		return 0;
	}

	return FMath::Lerp(InAirSettings.GroundPredictionAmountCurve->GetFloatValue(Hit.Time), 0.0f,
	                   GetCurveValueClamped01(Constants.GroundPredictionBlockCurve));
}

FAlsLeanState UAlsAnimationInstance::CalculateInAirLeanAmount() const
{
	// Use the relative velocity direction and amount to determine how much the character should lean
	// while in air. The lean amount curve gets the vertical velocity and is used as a multiplier to
	// smoothly reverse the leaning direction when transitioning from moving upwards to moving downwards.

	const auto RelativeVelocity{
		LocomotionState.ActorRotation.UnrotateVector(LocomotionState.Velocity) / 350 *
		InAirSettings.LeanAmountCurve->GetFloatValue(InAirState.VerticalVelocity)
	};

	return {RelativeVelocity.Y, RelativeVelocity.X};
}

void UAlsAnimationInstance::RefreshRagdolling()
{
	if (LocomotionMode != EAlsLocomotionMode::Ragdolling)
	{
		return;
	}

	// Scale the flail play rate by the root speed. The faster the ragdoll moves, the faster the character will flail.

	RagdollingState.FlailPlayRate = UAlsMath::Clamp01(
		GetOwningComponent()->GetPhysicsLinearVelocity(Constants.RootBone).Size() / 1000
	);
}

void UAlsAnimationInstance::StopRagdolling()
{
	// Save a snapshot of the current ragdoll pose for use in animation graph to blend out of the ragdoll.

	SnapshotPose(RagdollingState.FinalRagdollPose);
}

float UAlsAnimationInstance::GetCurveValueClamped01(const FName CurveName) const
{
	return UAlsMath::Clamp01(GetCurveValue(CurveName));
}
