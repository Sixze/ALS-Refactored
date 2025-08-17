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

		// Get the foot reference rotation and convert it to local space relative to the calf.

		const auto CalfInitialRotation{Hierarchy->GetInitialGlobalTransform(CachedCalfItem).GetRotation()};
		const auto FootInitialRotation{Hierarchy->GetInitialGlobalTransform(CachedFootItem).GetRotation()};

		FootReferenceLocalRotation = CalfInitialRotation.Inverse() * FootInitialRotation;
	}

	OffsetNormal = UAlsMath::DamperExact(OffsetNormal, FootOffsetNormal,
	                                     UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), OffsetInterpolationHalfLife);

	const auto OffsetRotation{FQuat::FindBetweenVectors(FVector::ZAxisVector, OffsetNormal)};

	// Convert the global offset to a local offset.

	// 1. Transform the current foot rotation to local space relative to the calf rotation.
	// 2. Get delta quaternion from the reference foot rotation to the rotation from the previous step.
	// 3. Apply limit offset.

	const auto CurrentCalfRotation{Hierarchy->GetGlobalTransform(CachedCalfItem).GetRotation()};
	const auto CurrentFootRotation{Hierarchy->GetGlobalTransform(CachedFootItem).GetRotation()};

	const auto InitialLocalRotation{
		CurrentCalfRotation.Inverse() * CurrentFootRotation * FootReferenceLocalRotation.Inverse()
	};

	const auto TargetLocalRotation{
		CurrentCalfRotation.Inverse() * (OffsetRotation * FootTargetRotation) * FootReferenceLocalRotation.Inverse()
	};

	// We intentionally use FVector::XAxisVector here so that the twist is stored in the
	// X component of the quaternion, and the swing is stored in the Y and Z components.

	const FRotator InitialLocalRotator{InitialLocalRotation.Rotator()};
	const FRotator TargetLocalRotator{TargetLocalRotation.Rotator()};

	// Limit swing.

	static const auto ApplyConstraint{
		[](const float InitialAngle, const float TargetAngle, const FFloatInterval& LimitAngle)
		{
			// Initial rotation is the rotation of the foot that comes from animations. It must be taken
			// into account so that, for example, if the foot is rotated 45 degrees in the animation
			// and the limit angle is 25 degrees, the resulting foot rotation will still be 45 degrees.

			const auto MinAngle{FMath::Min3(InitialAngle, LimitAngle.Min, LimitAngle.Max)};
			const auto MaxAngle{FMath::Max3(InitialAngle, LimitAngle.Min, LimitAngle.Max)};

			return FMath::Clamp(TargetAngle, MinAngle, MaxAngle);
		}
	};

	const auto NewPitch{
		ApplyConstraint(UE_REAL_TO_FLOAT(InitialLocalRotator.Pitch), UE_REAL_TO_FLOAT(TargetLocalRotator.Pitch), Swing2LimitAngle)
	};

	const auto NewYaw{
		ApplyConstraint(UE_REAL_TO_FLOAT(InitialLocalRotator.Yaw), UE_REAL_TO_FLOAT(TargetLocalRotator.Yaw), Swing1LimitAngle)
	};

	const auto NewRoll{
		ApplyConstraint(UE_REAL_TO_FLOAT(InitialLocalRotator.Roll), UE_REAL_TO_FLOAT(TargetLocalRotator.Roll), TwistLimitAngle)
	};

	// Convert the new local offset back to a global offset.

	const auto NewLocalRotation{FRotator{NewPitch, NewYaw, NewRoll}.Quaternion()};

	FootRotation = CurrentCalfRotation * (NewLocalRotation * FootReferenceLocalRotation);
	FootRotation.Normalize();
}
