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
	    !CachedCalfItem.UpdateCache(CalfItem, Hierarchy))
	{
		return;
	}

	if (!bInitialized)
	{
		bInitialized = true;

		OffsetNormal = FootOffsetNormal;
	}

	OffsetNormal = UAlsMath::ExponentialDecay(OffsetNormal, FootOffsetNormal,
	                                          UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), OffsetInterpolationSpeed);

	const auto OffsetRotation{FQuat::FindBetweenVectors(FVector::ZAxisVector, OffsetNormal)};

	// Convert global offset to local offset.

	const auto CalfRotation{Hierarchy->GetGlobalTransform(CachedCalfItem).GetRotation()};

	const auto InitialLocalRotation{CalfRotation.Inverse() * FootTargetRotation * LimitOffset};

	const auto TargetRotation{OffsetRotation * FootTargetRotation};
	const auto TargetLocalRotation{CalfRotation.Inverse() * TargetRotation * LimitOffset};

	// We intentionally use FVector::XAxisVector here so that the twist is stored in the
	// X component of the quaternion, and the swing is stored in the Y and Z components.

	FQuat InitialSwing;
	FQuat InitialTwist;
	InitialLocalRotation.ToSwingTwist(FVector::XAxisVector, InitialSwing, InitialTwist);

	FQuat TargetSwing;
	FQuat TargetTwist;
	TargetLocalRotation.ToSwingTwist(FVector::XAxisVector, TargetSwing, TargetTwist);

	// Limit swing.

	static const auto ApplyConstraint{
		[](const float Initial, const float Target, const float LimitAngle)
		{
			const auto Limit{FMath::Sin(FMath::DegreesToRadians(LimitAngle) * 0.5f)};

			float Min;
			float Max;

			// Initial rotation is the rotation of the foot that comes from animations. It must be taken
			// into account so that, for example, if the foot is rotated 45 degrees in the animation
			// and the limit angle is 25 degrees, the resulting foot rotation will still be 45 degrees.

			if (Initial >= 0.0f)
			{
				Min = -Limit;
				Max = FMath::Max(Initial, Limit);
			}
			else
			{
				Min = FMath::Min(Initial, -Limit);
				Max = Limit;
			}

			return FMath::Clamp(Target, Min, Max);
		}
	};

	const auto NewSwingY{ApplyConstraint(UE_REAL_TO_FLOAT(InitialSwing.Y), UE_REAL_TO_FLOAT(TargetSwing.Y), Swing2LimitAngle)};
	const auto NewSwingZ{ApplyConstraint(UE_REAL_TO_FLOAT(InitialSwing.Z), UE_REAL_TO_FLOAT(TargetSwing.Z), Swing1LimitAngle)};

	const FQuat NewSwing{
		0.0f, NewSwingY, NewSwingZ,
		FMath::Sign(TargetSwing.W) * FMath::Sqrt(FMath::Max(0.0f, 1.0f - FMath::Square(NewSwingY) - FMath::Square(NewSwingZ)))
	};

	// Limit twist.

	const auto NewTwistX{ApplyConstraint(UE_REAL_TO_FLOAT(InitialTwist.X), UE_REAL_TO_FLOAT(TargetTwist.X), TwistLimitAngle)};

	const FQuat NewTwist{
		NewTwistX, 0.0f, 0.0f,
		FMath::Sign(TargetTwist.W) * FMath::Sqrt(FMath::Max(0.0f, 1.0f - FMath::Square(NewTwistX)))
	};

	// Convert the new local offset back to a global offset.

	const auto NewLocalRotation{NewSwing * NewTwist};

	FootRotation = CalfRotation * NewLocalRotation * LimitOffset.Inverse();
	FootRotation.Normalize();
}
