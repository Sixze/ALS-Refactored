#include "Nodes/AlsRigUnit_ApplyFootOffsetRotation.h"

#include "Engine/HitResult.h"
#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_ApplyFootOffsetRotation)

void FAlsRigUnit_ApplyFootOffsetRotation::Initialize()
{
	bInitialized = false;
}

FAlsRigUnit_ApplyFootOffsetRotation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto* Hierarchy{ExecuteContext.Hierarchy};

	if (!IsValid(Hierarchy) ||
	    !CachedCalfItem.UpdateCache(CalfItem, Hierarchy) ||
	    !CachedFootItem.UpdateCache(FootItem, Hierarchy))
	{
		return;
	}

	if (!bInitialized)
	{
		bInitialized = true;

		OffsetNormal = FootOffsetNormal;

		// Get the foot initial rotation and convert it to the calf space.

		const auto CalfInitialRotation{Hierarchy->GetInitialGlobalTransform(CachedCalfItem).GetRotation()};
		const auto FootInitialRotation{Hierarchy->GetInitialGlobalTransform(CachedFootItem).GetRotation()};

		FootInitialRotationCalfSpace = CalfInitialRotation.Inverse() * FootInitialRotation;
	}

	OffsetNormal = UAlsMath::DamperExact(OffsetNormal, FootOffsetNormal,
	                                     UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), OffsetInterpolationHalfLife);

	const auto OffsetRotation{FQuat::FindBetweenVectors(FVector::UpVector, OffsetNormal)};

	// Transform the current foot rotation and the target foot rotation with applied foot offset to the calf space.

	const auto CalfRotation{Hierarchy->GetGlobalTransform(CachedCalfItem).GetRotation()};
	FootRotation = Hierarchy->GetGlobalTransform(CachedFootItem).GetRotation();

	const auto CurrentRotationCalfSpace{
		(CalfRotation.Inverse() * FootRotation * FootInitialRotationCalfSpace.Inverse()).Rotator()
	};

	const auto TargetRotationCalfSpace{
		(CalfRotation.Inverse() * (OffsetRotation * FootTargetRotation) * FootInitialRotationCalfSpace.Inverse()).Rotator()
	};

	// Apply limits.

	static const auto ConstraintTargetAngle{
		[](const float CurrentAngle, const float TargetAngle, const FFloatInterval& LimitAngle)
		{
			// Initial rotation is the rotation of the foot that comes from animations. It must be taken
			// into account so that, for example, if the foot is rotated 45 degrees in the animation
			// and the limit angle is 25 degrees, the resulting foot rotation will still be 45 degrees.

			const auto MinAngle{FMath::Min3(CurrentAngle, LimitAngle.Min, LimitAngle.Max)};
			const auto MaxAngle{FMath::Max3(CurrentAngle, LimitAngle.Min, LimitAngle.Max)};

			return FMath::Clamp(TargetAngle, MinAngle, MaxAngle);
		}
	};

	const auto FinalPitchAngleCalfSpace{
		ConstraintTargetAngle(UE_REAL_TO_FLOAT(CurrentRotationCalfSpace.Pitch),
		                      UE_REAL_TO_FLOAT(TargetRotationCalfSpace.Pitch),
		                      Swing2LimitAngle)
	};

	const auto FinalYawAngleCalfSpace{
		ConstraintTargetAngle(UE_REAL_TO_FLOAT(CurrentRotationCalfSpace.Yaw),
		                      UE_REAL_TO_FLOAT(TargetRotationCalfSpace.Yaw),
		                      Swing1LimitAngle)
	};

	const auto FinalRollAngleCalfSpace{
		ConstraintTargetAngle(UE_REAL_TO_FLOAT(CurrentRotationCalfSpace.Roll),
		                      UE_REAL_TO_FLOAT(TargetRotationCalfSpace.Roll),
		                      TwistLimitAngle)
	};

	const auto FinalRotationCalfSpace{FRotator{FinalPitchAngleCalfSpace, FinalYawAngleCalfSpace, FinalRollAngleCalfSpace}.Quaternion()};

	// Convert the new calf space foot rotation back to the global space.

	FootRotation = CalfRotation * (FinalRotationCalfSpace * FootInitialRotationCalfSpace);
	FootRotation.Normalize();
}
