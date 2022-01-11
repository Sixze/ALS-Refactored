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

void UAlsAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AlsCharacter = Cast<AAlsCharacter>(GetOwningActor());
}

void UAlsAnimationInstance::NativeBeginPlay()
{
	check(IsValid(Settings));
	check(IsValid(AlsCharacter));

	Super::NativeBeginPlay();
}

void UAlsAnimationInstance::NativeUpdateAnimation(const float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(Settings) || !IsValid(AlsCharacter))
	{
		return;
	}

	const auto PreviousLocomotionAction{LocomotionAction};

	Stance = AlsCharacter->GetStance();
	Gait = AlsCharacter->GetGait();
	RotationMode = AlsCharacter->GetRotationMode();
	ViewMode = AlsCharacter->GetViewMode();
	OverlayMode = AlsCharacter->GetOverlayMode();
	LocomotionMode = AlsCharacter->GetLocomotionMode();
	LocomotionAction = AlsCharacter->GetLocomotionAction();

	if (PreviousLocomotionAction == FAlsLocomotionActionTags::Get().Ragdolling &&
	    LocomotionAction != FAlsLocomotionActionTags::Get().Ragdolling)
	{
		StopRagdolling();
	}

	RefreshLocomotion(DeltaTime);
	RefreshLayering();
	RefreshPose();
	RefreshView(DeltaTime);

	RefreshFeet(DeltaTime);

	RefreshMovement(DeltaTime);
	RefreshDynamicTransitions();

	RefreshRotateInPlace(DeltaTime);
	RefreshTurnInPlace(DeltaTime);

	RefreshInAir(DeltaTime);
	RefreshRagdolling();

	SetPendingUpdate(false);
	SetRotationYawSpeedChanged(true);
}

void UAlsAnimationInstance::RefreshLocomotion(const float DeltaTime)
{
	LocomotionState.bHasInput = AlsCharacter->GetLocomotionState().bHasInput;

	if (LocomotionState.bHasInput && RotationMode.IsVelocityDirection())
	{
		// Get the delta between character rotation and current input yaw angle and map it to a range from
		// 0 to 1. This value is used in the aiming to make the character look toward the current input.

		const auto InputYawAngle{
			FRotator::NormalizeAxis(AlsCharacter->GetLocomotionState().InputYawAngle - AlsCharacter->GetLocomotionState().Rotation.Yaw)
		};

		const auto InputYawAmount{(InputYawAngle / 180.0f + 1.0f) * 0.5f};

		LocomotionState.InputYawAmount = UAlsMath::ExponentialDecay(LocomotionState.InputYawAmount, InputYawAmount, DeltaTime,
		                                                            Settings->General.InputYawAmountInterpolationSpeed);
	}

	LocomotionState.bHasSpeed = AlsCharacter->GetLocomotionState().bHasSpeed;
	LocomotionState.Speed = AlsCharacter->GetLocomotionState().Speed;
	LocomotionState.Velocity = AlsCharacter->GetLocomotionState().Velocity;
	LocomotionState.VelocityYawAngle = AlsCharacter->GetLocomotionState().VelocityYawAngle;

	LocomotionState.bMoving = AlsCharacter->GetLocomotionState().bMoving;
	LocomotionState.bMovingSmooth = LocomotionState.bHasInput || LocomotionState.Speed > Settings->General.MovingSpeedSmoothThreshold;

	LocomotionState.GaitAmount = GetCurveValue(UAlsConstants::GaitAmountCurve());
	LocomotionState.GaitWalkingAmount = UAlsMath::Clamp01(LocomotionState.GaitAmount);
	LocomotionState.GaitRunningAmount = UAlsMath::Clamp01(LocomotionState.GaitAmount - 1.0f);
	LocomotionState.GaitSprintingAmount = UAlsMath::Clamp01(LocomotionState.GaitAmount - 2.0f);

	// The allow transitions curve is modified within certain states, so that allow transition will be true while in those states.

	LocomotionState.bAllowTransitions = FAnimWeight::IsFullWeight(GetCurveValue(UAlsConstants::AllowTransitionsCurve()));

	// Allow movement animations if character is moving.

	if (!LocomotionState.bMoving || !LocomotionMode.IsGrounded())
	{
		return;
	}

	// Calculate the relative acceleration amount. This value represents the current amount of acceleration / deceleration
	// relative to the actor rotation. It is normalized to a range of -1 to 1 so that -1 equals the
	// max braking deceleration and 1 equals the max acceleration of the character movement component.

	const auto Acceleration{AlsCharacter->GetLocomotionState().Acceleration};

	const auto* CharacterMovement{AlsCharacter->GetCharacterMovement()};

	if ((Acceleration | LocomotionState.Velocity) >= 0.0f)
	{
		LocomotionState.RelativeAccelerationAmount = AlsCharacter->GetLocomotionState().Rotation.UnrotateVector(
			UAlsMath::ClampMagnitude01(Acceleration / CharacterMovement->GetMaxAcceleration()));
	}
	else
	{
		LocomotionState.RelativeAccelerationAmount = AlsCharacter->GetLocomotionState().Rotation.UnrotateVector(
			UAlsMath::ClampMagnitude01(Acceleration / CharacterMovement->GetMaxBrakingDeceleration()));
	}

	// Set the rotation yaw offsets. These values influence the rotation yaw offset curve in the
	// animation graph and are used to offset the character's rotation for more natural movement.
	// The curves allow for fine control over how the offset behaves for each movement direction.

	const auto RotationYawOffset{
		FRotator::NormalizeAxis(LocomotionState.VelocityYawAngle - AlsCharacter->GetViewState().SmoothRotation.Yaw)
	};

	LocomotionState.RotationYawOffsetForward = Settings->General.RotationYawOffsetForwardCurve->GetFloatValue(RotationYawOffset);
	LocomotionState.RotationYawOffsetBackward = Settings->General.RotationYawOffsetBackwardCurve->GetFloatValue(RotationYawOffset);
	LocomotionState.RotationYawOffsetLeft = Settings->General.RotationYawOffsetLeftCurve->GetFloatValue(RotationYawOffset);
	LocomotionState.RotationYawOffsetRight = Settings->General.RotationYawOffsetRightCurve->GetFloatValue(RotationYawOffset);

	// Use the relative acceleration as the sprint relative acceleration if less than 0.5 seconds has elapsed
	// since the start of the sprint, otherwise set the sprint relative acceleration to zero.This is
	// necessary in order to apply the acceleration animation only at the beginning of the sprint.

	if (Gait.IsSprinting())
	{
		LocomotionState.SprintTime += DeltaTime;
		LocomotionState.SprintRelativeAccelerationAmount = LocomotionState.SprintTime < 0.5f
			                                                   ? LocomotionState.RelativeAccelerationAmount.X
			                                                   : 0.0f;
	}
	else
	{
		LocomotionState.SprintTime = 0.0f;
		LocomotionState.SprintRelativeAccelerationAmount = 0.0f;
	}

	LocomotionState.SprintBlockAmount = GetCurveValueClamped01(UAlsConstants::SprintBlockCurve());
}

void UAlsAnimationInstance::RefreshLayering()
{
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
	PoseState.MovingAmount = GetCurveValueClamped01(UAlsConstants::PoseMovingCurve());
	PoseState.StandingAmount = GetCurveValueClamped01(UAlsConstants::PoseStandingCurve());
	PoseState.CrouchingAmount = GetCurveValueClamped01(UAlsConstants::PoseCrouchingCurve());
	PoseState.GroundedAmount = GetCurveValueClamped01(UAlsConstants::PoseGroundedCurve());
	PoseState.InAirAmount = GetCurveValueClamped01(UAlsConstants::PoseInAirCurve());
}

void UAlsAnimationInstance::RefreshView(const float DeltaTime)
{
	ViewState.Rotation = AlsCharacter->GetViewState().SmoothRotation;

	if (!LocomotionAction.IsValid())
	{
		ViewState.YawAngle = FRotator::NormalizeAxis(ViewState.Rotation.Yaw - AlsCharacter->GetLocomotionState().Rotation.Yaw);
		ViewState.PitchAngle = FRotator::NormalizeAxis(ViewState.Rotation.Pitch - AlsCharacter->GetLocomotionState().Rotation.Pitch);
	}

	ViewState.YawSpeed = AlsCharacter->GetViewState().YawSpeed;

	// Interpolate the view rotation value to achieve smooth view rotation changes. Interpolating
	// the rotation before calculating the angle ensures the value is not affected by changes in
	// actor rotation, allowing slow view rotation changes with fast actor rotation changes.

	ViewState.SmoothRotation = UAlsMath::ExponentialDecay(ViewState.SmoothRotation, ViewState.Rotation, DeltaTime,
	                                                      Settings->General.ViewSmoothRotationInterpolationSpeed);
	ViewState.SmoothRotation.Normalize();

	ViewState.SmoothYawAngle = FRotator::NormalizeAxis(ViewState.SmoothRotation.Yaw -
	                                                   AlsCharacter->GetLocomotionState().Rotation.Yaw);

	ViewState.SmoothPitchAngle = FRotator::NormalizeAxis(ViewState.SmoothRotation.Pitch -
	                                                     AlsCharacter->GetLocomotionState().Rotation.Pitch);

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

	if (IsSpineRotationAllowed())
	{
		ViewState.SpineYawAngle = ViewState.YawAngle > 180.0f - UAlsMath::CounterClockwiseRotationAngleThreshold
			                          ? ViewState.YawAngle - 360.0f
			                          : ViewState.YawAngle;
	}

	const auto AimAllowedAmount{1.0f - GetCurveValueClamped01(UAlsConstants::AimBlockCurve())};
	const auto AimManualAmount{GetCurveValueClamped01(UAlsConstants::AimManualCurve())};

	ViewState.SpineYawAngle = FRotator::NormalizeAxis(ViewState.SpineYawAngle * AimAllowedAmount * AimManualAmount);
	ViewState.LookAmount = AimAllowedAmount * (1.0f - AimManualAmount);
}

bool UAlsAnimationInstance::IsSpineRotationAllowed()
{
	return RotationMode.IsAiming();
}

void UAlsAnimationInstance::RefreshFeet(const float DeltaTime)
{
	if (bPendingUpdate)
	{
		// Reinitialize foot locking.

		FeetState.BasePrimitive = nullptr;
		FeetState.Left.LockLocation = FVector::ZeroVector;
		FeetState.Right.LockLocation = FVector::ZeroVector;
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

	const auto RelativeTransform{GetSkelMeshComponent()->GetComponentTransform().Inverse()};

	const auto& BasedMovement{AlsCharacter->GetBasedMovement()};
	if (BasedMovement.MovementBase != FeetState.BasePrimitive || BasedMovement.BoneName != FeetState.BaseBoneName)
	{
		FeetState.BasePrimitive = BasedMovement.MovementBase;
		FeetState.BaseBoneName = BasedMovement.BoneName;

		FVector BaseLocation;
		FQuat BaseRotation;
		MovementBaseUtility::GetMovementBaseTransform(BasedMovement.MovementBase, BasedMovement.BoneName, BaseLocation, BaseRotation);

		HandleFootLockChangedBase(FeetState.Left, FootLeftBone, BaseLocation, BaseRotation, RelativeTransform);
		HandleFootLockChangedBase(FeetState.Right, FootRightBone, BaseLocation, BaseRotation, RelativeTransform);
	}

	FVector FinalFootLeftLocation;
	FQuat FinalFootLeftRotation;
	RefreshFootLock(FeetState.Left, FootLeftBone, UAlsConstants::FootLeftLockCurve(),
	                RelativeTransform, DeltaTime, FinalFootLeftLocation, FinalFootLeftRotation);

	FVector FinalFootRightLocation;
	FQuat FinalFootRightRotation;
	RefreshFootLock(FeetState.Right, FootRightBone, UAlsConstants::FootRightLockCurve(),
	                RelativeTransform, DeltaTime, FinalFootRightLocation, FinalFootRightRotation);

	if (LocomotionMode.IsInAir())
	{
		ResetFootOffset(FeetState.Left, DeltaTime, FinalFootLeftLocation, FinalFootLeftRotation);
		ResetFootOffset(FeetState.Right, DeltaTime, FinalFootRightLocation, FinalFootRightRotation);

		RefreshFinalFootState(FeetState.Left, RelativeTransform, FinalFootLeftLocation, FinalFootLeftRotation);
		RefreshFinalFootState(FeetState.Right, RelativeTransform, FinalFootRightLocation, FinalFootRightRotation);

		RefreshPelvisOffset(DeltaTime, 0.0f, 0.0f);
		return;
	}

	if (LocomotionAction == FAlsLocomotionActionTags::Get().Ragdolling)
	{
		RefreshFinalFootState(FeetState.Left, RelativeTransform, FinalFootLeftLocation, FinalFootLeftRotation);
		RefreshFinalFootState(FeetState.Right, RelativeTransform, FinalFootRightLocation, FinalFootRightRotation);
		return;
	}

	FVector TargetFootLeftLocationOffset;
	RefreshFootOffset(FeetState.Left, DeltaTime, TargetFootLeftLocationOffset, FinalFootLeftLocation, FinalFootLeftRotation);

	FVector TargetFootRightLocationOffset;
	RefreshFootOffset(FeetState.Right, DeltaTime, TargetFootRightLocationOffset, FinalFootRightLocation, FinalFootRightRotation);

	RefreshFinalFootState(FeetState.Left, RelativeTransform, FinalFootLeftLocation, FinalFootLeftRotation);
	RefreshFinalFootState(FeetState.Right, RelativeTransform, FinalFootRightLocation, FinalFootRightRotation);

	RefreshPelvisOffset(DeltaTime, TargetFootLeftLocationOffset.Z, TargetFootRightLocationOffset.Z);
}

void UAlsAnimationInstance::HandleFootLockChangedBase(FAlsFootState& FootState, const FName& FootBoneName, const FVector& BaseLocation,
                                                      const FQuat& BaseRotation, const FTransform& RelativeTransform) const
{
	if (FootState.LockLocation.IsZero())
	{
		const auto FootTransform{GetSkelMeshComponent()->GetSocketTransform(FootBoneName)};

		FootState.LockLocation = FootTransform.GetLocation();
		FootState.LockRotation = FootTransform.GetRotation();
	}

	const auto& BasedMovement{AlsCharacter->GetBasedMovement()};
	if (BasedMovement.HasRelativeLocation())
	{
		const auto BaseRotationInverted{BaseRotation.Inverse()};

		FootState.LockRelativeLocation = BaseRotationInverted.RotateVector(FootState.LockLocation - BaseLocation);
		FootState.LockRelativeRotation = BaseRotationInverted * FootState.LockRotation;
	}
	else if (IsValid(BasedMovement.MovementBase))
	{
		FootState.LockRelativeLocation = FootState.LockLocation;
		FootState.LockRelativeRotation = FootState.LockRotation;
	}
	else
	{
		FootState.LockRelativeLocation = RelativeTransform.TransformPosition(FootState.LockLocation);
		FootState.LockRelativeRotation = RelativeTransform.TransformRotation(FootState.LockRotation);
	}
}

void UAlsAnimationInstance::RefreshFootLock(FAlsFootState& FootState, const FName& FootBoneName,
                                            const FName& FootLockCurveName, const FTransform& RelativeTransform,
                                            const float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation) const
{
	if (!FAnimWeight::IsRelevant(FootState.IkAmount))
	{
		return;
	}

	const auto FootTransform{GetSkelMeshComponent()->GetSocketTransform(FootBoneName)};
	auto NewFootLockAmount{GetCurveValueClamped01(FootLockCurveName)};

	if (LocomotionMode.IsInAir() && NewFootLockAmount > FootState.LockAmount)
	{
		// Smoothly disable foot locking if the character is in the air.

		NewFootLockAmount = FMath::Max(0.0f, FootState.LockAmount - DeltaTime * 0.6f);
	}

	if (Settings->Feet.bDisableFootLock || !FAnimWeight::IsRelevant(NewFootLockAmount))
	{
		FootState.LockAmount = 0.0f;

		FootState.LockLocation = FootTransform.GetLocation();
		FootState.LockRotation = FootTransform.GetRotation();

		FinalLocation = FootState.LockLocation;
		FinalRotation = FootState.LockRotation;
		return;
	}

	const auto& BasedMovement{AlsCharacter->GetBasedMovement()};

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
			if (BasedMovement.HasRelativeLocation())
			{
				const auto BaseRotationInverted{BaseRotation.Inverse()};

				FootState.LockRelativeLocation = BaseRotationInverted.RotateVector(FootTransform.GetLocation() - BaseLocation);
				FootState.LockRelativeRotation = BaseRotationInverted * FootTransform.GetRotation();
			}
			else if (IsValid(BasedMovement.MovementBase))
			{
				FootState.LockRelativeLocation = FootTransform.GetLocation();
				FootState.LockRelativeRotation = FootTransform.GetRotation();
			}
			else
			{
				if (FootState.LockLocation.IsZero())
				{
					FootState.LockLocation = FootTransform.GetLocation();
					FootState.LockRotation = FootTransform.GetRotation();
				}

				FootState.LockRelativeLocation = RelativeTransform.TransformPosition(FootState.LockLocation);
				FootState.LockRelativeRotation = RelativeTransform.TransformRotation(FootState.LockRotation);
			}
		}
	}

	if (BasedMovement.HasRelativeLocation())
	{
		FootState.LockLocation = BaseLocation + BaseRotation.RotateVector(FootState.LockRelativeLocation);
		FootState.LockRotation = BaseRotation * FootState.LockRelativeRotation;
	}
	else if (IsValid(BasedMovement.MovementBase))
	{
		FootState.LockLocation = FootState.LockRelativeLocation;
		FootState.LockRotation = FootState.LockRelativeRotation;
	}
	else
	{
		const auto& ComponentTransform{GetSkelMeshComponent()->GetComponentTransform()};

		FootState.LockLocation = ComponentTransform.TransformPosition(FootState.LockRelativeLocation);
		FootState.LockRotation = ComponentTransform.TransformRotation(FootState.LockRelativeRotation);
	}

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

	// Trace downward from the foot location to find the geometry. If the surface is walkable, save the impact location and normal.

	auto FootLocation{FinalLocation};
	FootLocation.Z = GetSkelMeshComponent()->GetSocketLocation(UAlsConstants::RootBone()).Z;

	FCollisionQueryParams QueryParameters{ANSI_TO_TCHAR(__FUNCTION__), true, AlsCharacter};
	QueryParameters.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit,
	                                     FootLocation + FVector{0.0f, 0.0f, Settings->Feet.IkTraceDistanceUpward},
	                                     FootLocation - FVector{0.0f, 0.0f, Settings->Feet.IkTraceDistanceDownward},
	                                     UEngineTypes::ConvertToCollisionChannel(Settings->Feet.IkTraceChannel),
	                                     QueryParameters);

#if ENABLE_DRAW_DEBUG
	if (UAlsUtility::ShouldDisplayDebug(AlsCharacter, UAlsConstants::TracesDisplayName()))
	{
		UAlsUtility::DrawDebugLineTraceSingle(GetWorld(), Hit.TraceStart, Hit.TraceEnd,
		                                      AlsCharacter->GetCharacterMovement()->IsWalkable(Hit),
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

	if (AlsCharacter->GetCharacterMovement()->IsWalkable(Hit))
	{
		// Find the difference in location from the impact location and the expected (flat) floor location. These values
		// are offset by the impact normal multiplied by the foot height to get better behavior on angled surfaces.

		TargetLocationOffset = Hit.ImpactPoint +
		                       Hit.ImpactNormal * Settings->Feet.FootHeight -
		                       FootLocation;

		TargetLocationOffset.Z -= Settings->Feet.FootHeight;

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

	FootState.OffsetLocation = UAlsMath::InterpolateVectorSpringStable(FootState.OffsetLocation, TargetLocationOffset,
	                                                                   FootState.OffsetSpringState, 20.0f, 4.0f, DeltaTime, 4.0f);

	FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, TargetRotationOffset, DeltaTime, 30.0f);

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;
}

void UAlsAnimationInstance::ResetFootOffset(FAlsFootState& FootState, const float DeltaTime, FVector& FinalLocation, FQuat& FinalRotation)
{
	FootState.OffsetLocation = UAlsMath::InterpolateVectorSpringStable(FootState.OffsetLocation, FVector::ZeroVector,
	                                                                   FootState.OffsetSpringState, 20.0f, 4.0f, DeltaTime, 4.0f);

	FootState.OffsetRotation = FMath::QInterpTo(FootState.OffsetRotation, FQuat::Identity, DeltaTime, 15.0f);

	FinalLocation += FootState.OffsetLocation;
	FinalRotation = FootState.OffsetRotation * FinalRotation;

	FootState.bOffsetHitValid = false;
}

void UAlsAnimationInstance::RefreshFinalFootState(FAlsFootState& FootState, const FTransform& RelativeTransform,
                                                  const FVector& FinalLocation, const FQuat& FinalRotation)
{
	FootState.FinalLocation = FinalLocation;
	FootState.FinalRotation = FinalRotation;

	FootState.FinalRelativeLocation = RelativeTransform.TransformPosition(FinalLocation);
	FootState.FinalRelativeRotation = RelativeTransform.TransformRotation(FinalRotation);
}

void UAlsAnimationInstance::RefreshPelvisOffset(const float DeltaTime, const float TargetFootLeftLocationOffsetZ,
                                                const float TargetFootRightLocationOffsetZ)
{
	// Calculate the pelvis offset amount by finding the average foot ik weight.

	FeetState.PelvisOffsetAmount = (FeetState.Left.IkAmount + FeetState.Right.IkAmount) * 0.5f;

	// Set the new offset to be the lowest foot offset.

	const auto TargetPelvisOffsetZ{FMath::Min(TargetFootLeftLocationOffsetZ, TargetFootRightLocationOffsetZ)};

	// Interpolate current offset to the new target value.

	FeetState.PelvisOffsetZ = UAlsMath::InterpolateFloatSpringStable(FeetState.PelvisOffsetZ, TargetPelvisOffsetZ,
	                                                                 FeetState.PelvisSpringState, 10.0f, 2.0f, DeltaTime, 10.0f);
}

void UAlsAnimationInstance::RefreshMovement(const float DeltaTime)
{
	MovementState.HipsDirectionLockAmount = FMath::Clamp(GetCurveValue(UAlsConstants::HipsDirectionLockCurve()), -1.0f, 1.0f);

	if (!LocomotionMode.IsGrounded() || !LocomotionState.bMoving)
	{
		return;
	}

	MovementDirection = CalculateMovementDirection();

	RefreshVelocityBlend(DeltaTime);

	MovementState.StrideBlendAmount = CalculateStrideBlendAmount();
	MovementState.WalkRunBlendAmount = CalculateWalkRunBlendAmount();
	MovementState.StandingPlayRate = CalculateStandingPlayRate();
	MovementState.CrouchingPlayRate = CalculateCrouchingPlayRate();

	// Interpolate the lean amount.

	LeanState.RightAmount = FMath::FInterpTo(LeanState.RightAmount, LocomotionState.RelativeAccelerationAmount.Y,
	                                         DeltaTime, Settings->General.LeanInterpolationSpeed);

	LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, LocomotionState.RelativeAccelerationAmount.X,
	                                           DeltaTime, Settings->General.LeanInterpolationSpeed);
}

EAlsMovementDirection UAlsAnimationInstance::CalculateMovementDirection() const
{
	// Calculate the movement direction. This value represents the direction the character is moving relative to the camera during
	// the looking direction / aiming modes and is used in the cycle blending to blend to the appropriate directional states.

	if (Gait.IsSprinting() || RotationMode.IsVelocityDirection())
	{
		return EAlsMovementDirection::Forward;
	}

	return UAlsMath::CalculateMovementDirection(
		FRotator::NormalizeAxis(LocomotionState.VelocityYawAngle - ViewState.Rotation.Yaw), 70.0f, 5.0f);
}

void UAlsAnimationInstance::RefreshVelocityBlend(const float DeltaTime)
{
	// Calculate and interpolate the velocity blend. This value represents the velocity amount of the
	// actor in each direction (normalized so that diagonals equal 0.5 for each direction) and is
	// used in a blend multi node to produce better directional blending than a standard blend space.

	const auto RelativeVelocityDirection{
		AlsCharacter->GetLocomotionState().Rotation.UnrotateVector(LocomotionState.Velocity.GetSafeNormal())
	};

	const auto RelativeDirection{
		RelativeVelocityDirection /
		(FMath::Abs(RelativeVelocityDirection.X) +
		 FMath::Abs(RelativeVelocityDirection.Y) +
		 FMath::Abs(RelativeVelocityDirection.Z))
	};

	MovementState.VelocityBlend.ForwardAmount = FMath::FInterpTo(MovementState.VelocityBlend.ForwardAmount,
	                                                             UAlsMath::Clamp01(RelativeDirection.X), DeltaTime,
	                                                             Settings->Movement.VelocityBlendInterpolationSpeed);

	MovementState.VelocityBlend.BackwardAmount = FMath::FInterpTo(MovementState.VelocityBlend.BackwardAmount,
	                                                              FMath::Abs(FMath::Clamp(RelativeDirection.X, -1.0f, 0.0f)), DeltaTime,
	                                                              Settings->Movement.VelocityBlendInterpolationSpeed);

	MovementState.VelocityBlend.LeftAmount = FMath::FInterpTo(MovementState.VelocityBlend.LeftAmount,
	                                                          FMath::Abs(FMath::Clamp(RelativeDirection.Y, -1.0f, 0.0f)), DeltaTime,
	                                                          Settings->Movement.VelocityBlendInterpolationSpeed);

	MovementState.VelocityBlend.RightAmount = FMath::FInterpTo(MovementState.VelocityBlend.RightAmount,
	                                                           UAlsMath::Clamp01(RelativeDirection.Y), DeltaTime,
	                                                           Settings->Movement.VelocityBlendInterpolationSpeed);
}

float UAlsAnimationInstance::CalculateStrideBlendAmount() const
{
	// Calculate the stride blend. This value is used within the blend spaces to scale the stride (distance feet travel) so
	// that the character can walk or run at different movement speeds. It also allows the walk or run gait animations to
	// blend independently while still matching the animation speed to the movement speed, preventing the character from needing
	// to play a half walk + half run blend. The curves are used to map the stride amount to the speed for maximum control.

	const auto Speed{LocomotionState.Speed / GetSkelMeshComponent()->GetComponentScale().Z};

	const auto StandingStrideBlend{
		FMath::Lerp(Settings->Movement.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
		            Settings->Movement.StrideBlendAmountRunCurve->GetFloatValue(Speed),
		            LocomotionState.GaitRunningAmount)
	};

	// Crouching stride blends.

	return FMath::Lerp(StandingStrideBlend,
	                   Settings->Movement.StrideBlendAmountWalkCurve->GetFloatValue(Speed),
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
		FMath::Lerp(LocomotionState.Speed / Settings->Movement.AnimatedWalkSpeed,
		            LocomotionState.Speed / Settings->Movement.AnimatedRunSpeed,
		            LocomotionState.GaitRunningAmount)
	};

	const auto WalkRunSprintSpeedAmount{
		FMath::Lerp(WalkRunSpeedAmount,
		            LocomotionState.Speed / Settings->Movement.AnimatedSprintSpeed,
		            LocomotionState.GaitSprintingAmount)
	};

	return FMath::Clamp(WalkRunSprintSpeedAmount / MovementState.StrideBlendAmount / GetSkelMeshComponent()->GetComponentScale().Z, 0.0f,
	                    3.0f);
}

float UAlsAnimationInstance::CalculateCrouchingPlayRate() const
{
	// Calculate the crouching play rate by dividing the character's speed by the animated speed. This value needs
	// to be separate from the standing play rate to improve the blend from crouching to standing while in motion.

	return FMath::Clamp(LocomotionState.Speed / Settings->Movement.AnimatedCrouchSpeed /
	                    MovementState.StrideBlendAmount / GetSkelMeshComponent()->GetComponentScale().Z,
	                    0.0f, 2.0f);
}

void UAlsAnimationInstance::ActivatePivot()
{
	MovementState.bPivotActive = LocomotionState.Speed < Settings->Movement.PivotActivationSpeedThreshold;

	if (MovementState.bPivotActive)
	{
		GetWorld()->GetTimerManager().SetTimer(PivotResetTimer,
		                                       FTimerDelegate::CreateWeakLambda(this, [&bPivotActive = MovementState.bPivotActive]
		                                       {
			                                       bPivotActive = false;
		                                       }), 0.1f, false);
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
	if (!LocomotionState.bMoving && Stance.IsStanding())
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

void UAlsAnimationInstance::RefreshDynamicTransitions()
{
	if (!LocomotionMode.IsGrounded() || LocomotionState.bMoving || !LocomotionState.bAllowTransitions)
	{
		return;
	}

	// Check each foot to see if the location difference between the foot look and its desired / target location
	// (determined via a ik bone) exceeds a threshold. If it does, play an additive transition animation on that
	// foot. The currently set transition plays the second half of a 2 foot transition animation, so that only
	// a single foot moves. The separate ik bone allows the system to know its desired location when locked.

	const auto SkeletalMesh{GetSkelMeshComponent()};

	if (FVector::DistSquared(SkeletalMesh->GetSocketLocation(Settings->General.bUseFootIkBones
		                                                         ? UAlsConstants::FootLeftIkBone()
		                                                         : UAlsConstants::FootLeftIkVirtualBone()),
	                         FeetState.Left.LockLocation) > FMath::Square(Settings->DynamicTransition.FootIkDistanceThreshold))
	{
		PlayDynamicTransition(SelectDynamicTransitionForRightFoot(), 0.2f, 0.2f, 1.5f, 0.8f, 0.1f);
		return;
	}

	if (FVector::DistSquared(SkeletalMesh->GetSocketLocation(Settings->General.bUseFootIkBones
		                                                         ? UAlsConstants::FootRightIkBone()
		                                                         : UAlsConstants::FootRightIkVirtualBone()),
	                         FeetState.Right.LockLocation) > FMath::Square(Settings->DynamicTransition.FootIkDistanceThreshold))
	{
		PlayDynamicTransition(SelectDynamicTransitionForLeftFoot(), 0.2f, 0.2f, 1.5f, 0.8f, 0.1f);
	}
}

void UAlsAnimationInstance::PlayDynamicTransition(UAnimSequenceBase* Animation, const float BlendInTime, const float BlendOutTime,
                                                  const float PlayRate, const float StartTime, const float AllowanceDelayTime)
{
	if (bAllowDynamicTransitions)
	{
		bAllowDynamicTransitions = false;

		GetWorld()->GetTimerManager().SetTimer(DynamicTransitionsAllowanceTimer,
		                                       FTimerDelegate::CreateWeakLambda(this, [&bAllowDynamicTransitions = bAllowDynamicTransitions]
		                                       {
			                                       bAllowDynamicTransitions = true;
		                                       }), AllowanceDelayTime, false);

		PlayTransition(Animation, BlendInTime, BlendOutTime, PlayRate, StartTime);
	}
}

bool UAlsAnimationInstance::IsRotateInPlaceAllowed()
{
	return RotationMode.IsAiming() || ViewMode == EAlsViewMode::FirstPerson;
}

void UAlsAnimationInstance::RefreshRotateInPlace(const float DeltaTime)
{
	// Rotate in place is allowed only if the character is standing still and aiming or in first-person view mode.

	if (!LocomotionMode.IsGrounded() || LocomotionState.bMoving || !IsRotateInPlaceAllowed())
	{
		RotateInPlaceState.bRotatingLeft = false;
		RotateInPlaceState.bRotatingRight = false;

		RotateInPlaceState.PlayRate = FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlace.PlayRate.X, DeltaTime, 5.0f);
		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// Check if the character should rotate left or right by checking if the view yaw angle exceeds the threshold.

	RotateInPlaceState.bRotatingLeft = ViewState.YawAngle < -Settings->RotateInPlace.ViewYawAngleThreshold;
	RotateInPlaceState.bRotatingRight = ViewState.YawAngle > Settings->RotateInPlace.ViewYawAngleThreshold;

	if (!RotateInPlaceState.bRotatingLeft && !RotateInPlaceState.bRotatingRight)
	{
		RotateInPlaceState.PlayRate = FMath::FInterpTo(RotateInPlaceState.PlayRate, Settings->RotateInPlace.PlayRate.X, DeltaTime, 5.0f);
		RotateInPlaceState.FootLockBlockAmount = 0.0f;
		return;
	}

	// If the character should be rotating, set the play rate to scale with the view yaw speed.
	// This makes the character rotate faster when moving the camera faster.

	const auto TargetPlayRate{
		FMath::GetMappedRangeValueClamped(Settings->RotateInPlace.ReferenceViewYawSpeed,
		                                  Settings->RotateInPlace.PlayRate, ViewState.YawSpeed)
	};

	RotateInPlaceState.PlayRate = FMath::FInterpTo(RotateInPlaceState.PlayRate, TargetPlayRate, DeltaTime, 5.0f);

	// Disable the foot lock when rotating at a large angle or rotating too fast, otherwise the legs may twist in a spiral.

	if (!Settings->RotateInPlace.bDisableFootLock)
	{
		RotateInPlaceState.FootLockBlockAmount = FMath::Abs(ViewState.YawAngle) > Settings->RotateInPlace.FootLockBlockViewYawAngleThreshold
			                                         ? 1.0f
			                                         : ViewState.YawSpeed > Settings->RotateInPlace.FootLockBlockViewYawSpeedThreshold
			                                         ? FMath::FInterpTo(RotateInPlaceState.FootLockBlockAmount, 1.0f, DeltaTime, 10.0f)
			                                         : 0.0f;
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

	if (!LocomotionMode.IsGrounded() || LocomotionState.bMoving || !IsTurnInPlaceAllowed())
	{
		TurnInPlaceState.ActivationDelayTime = 0.0f;
		TurnInPlaceState.bDisableFootLock = false;
		return;
	}

	if (!LocomotionState.bAllowTransitions)
	{
		TurnInPlaceState.ActivationDelayTime = 0.0f;
		return;
	}

	// Check if the view yaw speed is below the threshold and if view yaw angle is outside of the
	// threshold. If so, begin counting the activation delay time. If not, reset the activation delay time.
	// This ensures the conditions remain true for a sustained period of time before turning in place.

	if (ViewState.YawSpeed >= Settings->TurnInPlace.ViewYawSpeedThreshold ||
	    FMath::Abs(ViewState.YawAngle) <= Settings->TurnInPlace.ViewYawAngleThreshold)
	{
		TurnInPlaceState.ActivationDelayTime = 0.0f;
		TurnInPlaceState.bDisableFootLock = false;
		return;
	}

	TurnInPlaceState.ActivationDelayTime += DeltaTime;

	const auto ActivationDelay{
		FMath::GetMappedRangeValueClamped({Settings->TurnInPlace.ViewYawAngleThreshold, 180.0f},
		                                  Settings->TurnInPlace.ViewYawAngleToActivationDelay,
		                                  FMath::Abs(ViewState.YawAngle))
	};

	// Check if the activation delay time exceeds the set delay (mapped to the view yaw angle). If so, start a turn in place.

	if (TurnInPlaceState.ActivationDelayTime <= ActivationDelay)
	{
		return;
	}

	StartTurnInPlace(ViewState.Rotation.Yaw);
}

void UAlsAnimationInstance::StartTurnInPlace(const float TargetYawAngle, const float PlayRateScale, const float StartTime,
                                             const bool bAllowRestartIfPlaying)
{
	const auto TurnAngle{FRotator::NormalizeAxis(TargetYawAngle - AlsCharacter->GetLocomotionState().Rotation.Yaw)};

	// Choose settings on the turn angle and stance.

	FAlsTurnInPlaceSettings* TurnInPlaceSettings{nullptr};
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

	PlaySlotAnimationAsDynamicMontage(TurnInPlaceSettings->Animation, TurnInPlaceSlotName, 0.2f, 0.2f,
	                                  TurnInPlaceSettings->PlayRate * PlayRateScale, 1, 0.0f, StartTime);

	// Scale the rotation yaw delta (gets scaled in animation graph) to compensate for play rate and turn angle (if allowed).

	TurnInPlaceState.PlayRate = TurnInPlaceSettings->PlayRate * PlayRateScale;

	if (TurnInPlaceSettings->bScalePlayRateByAnimatedTurnAngle)
	{
		TurnInPlaceState.PlayRate *= FMath::Abs(TurnAngle / TurnInPlaceSettings->AnimatedTurnAngle);
	}

	TurnInPlaceState.bDisableFootLock = Settings->TurnInPlace.bDisableFootLock;
}

void UAlsAnimationInstance::Jump()
{
	InAirState.bJumped = true;
	InAirState.JumpPlayRate = UAlsMath::LerpClamped(1.2f, 1.5f, LocomotionState.Speed / 600.0f);

	GetWorld()->GetTimerManager().SetTimer(JumpResetTimer,
	                                       FTimerDelegate::CreateWeakLambda(this, [&bJumped = InAirState.bJumped]
	                                       {
		                                       bJumped = false;
	                                       }), 0.1f, false);
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
	                                         DeltaTime, Settings->General.LeanInterpolationSpeed);
	LeanState.ForwardAmount = FMath::FInterpTo(LeanState.ForwardAmount, TargetLeanAmount.ForwardAmount,
	                                           DeltaTime, Settings->General.LeanInterpolationSpeed);
}

float UAlsAnimationInstance::CalculateGroundPredictionAmount() const
{
	// Calculate the ground prediction weight by tracing in the velocity direction to find a walkable surface the character
	// is falling toward and getting the "time" (range from 0 to 1, 1 being maximum, 0 being about to ground) till impact.
	// The ground prediction amount curve is used to control how the time affects the final amount for a smooth blend.

	if (InAirState.VerticalVelocity >= -200.0f)
	{
		return 0.0f;
	}

	const auto* Capsule{AlsCharacter->GetCapsuleComponent()};
	const auto SweepStartLocation{AlsCharacter->GetLocomotionState().Location};

	auto VelocityDirection{LocomotionState.Velocity};
	VelocityDirection.Z = FMath::Clamp(InAirState.VerticalVelocity, -4000.0f, -200.0f);
	VelocityDirection.Normalize();

	const auto SweepVector{
		VelocityDirection * FMath::GetMappedRangeValueClamped({0.0f, -4000.0f}, {50.0f, 2000.0f}, InAirState.VerticalVelocity)
	};

	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : Settings->InAir.GroundPredictionSweepObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}

	FHitResult Hit;
	GetWorld()->SweepSingleByObjectType(Hit, SweepStartLocation, SweepStartLocation + SweepVector, FQuat::Identity, ObjectQueryParameters,
	                                    FCollisionShape::MakeCapsule(Capsule->GetScaledCapsuleRadius(),
	                                                                 Capsule->GetScaledCapsuleHalfHeight()),
	                                    {ANSI_TO_TCHAR(__FUNCTION__), false, AlsCharacter});

#if ENABLE_DRAW_DEBUG
	if (UAlsUtility::ShouldDisplayDebug(AlsCharacter, UAlsConstants::TracesDisplayName()))
	{
		UAlsUtility::DrawDebugSweepSingleCapsule(GetWorld(), Hit.TraceStart, Hit.TraceEnd, FRotator::ZeroRotator,
		                                         Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight(),
		                                         AlsCharacter->GetCharacterMovement()->IsWalkable(Hit),
		                                         Hit, {0.25f, 0.0f, 1.0f}, {0.75f, 0.0f, 1.0f});
	}
#endif

	if (!AlsCharacter->GetCharacterMovement()->IsWalkable(Hit))
	{
		return 0.0f;
	}

	return FMath::Lerp(Settings->InAir.GroundPredictionAmountCurve->GetFloatValue(Hit.Time), 0.0f,
	                   GetCurveValueClamped01(UAlsConstants::GroundPredictionBlockCurve()));
}

FAlsLeanState UAlsAnimationInstance::CalculateInAirLeanAmount() const
{
	// Use the relative velocity direction and amount to determine how much the character should lean
	// while in air. The lean amount curve gets the vertical velocity and is used as a multiplier to
	// smoothly reverse the leaning direction when transitioning from moving upwards to moving downwards.

	const auto RelativeVelocity{
		AlsCharacter->GetLocomotionState().Rotation.UnrotateVector(LocomotionState.Velocity) / 350.0f *
		Settings->InAir.LeanAmountCurve->GetFloatValue(InAirState.VerticalVelocity)
	};

	return {RelativeVelocity.Y, RelativeVelocity.X};
}

void UAlsAnimationInstance::RefreshRagdolling()
{
	if (LocomotionAction != FAlsLocomotionActionTags::Get().Ragdolling)
	{
		return;
	}

	// Scale the flail play rate by the root speed. The faster the ragdoll moves, the faster the character will flail.

	RagdollingState.FlailPlayRate = UAlsMath::Clamp01(
		GetSkelMeshComponent()->GetPhysicsLinearVelocity(UAlsConstants::RootBone()).Size() / 1000.0f
	);
}

void UAlsAnimationInstance::StopRagdolling()
{
	// Save a snapshot of the current ragdoll pose for use in animation graph to blend out of the ragdoll.

	SnapshotPose(RagdollingState.FinalRagdollPose);
}

float UAlsAnimationInstance::GetCurveValueClamped01(const FName& CurveName) const
{
	return UAlsMath::Clamp01(GetCurveValue(CurveName));
}
