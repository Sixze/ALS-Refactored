#include "Nodes/AlsRigUnit_ConstraintRotation.h"

#include "ControlRigDefines.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_ConstraintRotation)

FAlsRigUnit_ConstraintRotation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto RelativeRotation{CurrentRotation.Inverse() * TargetRotation};

	FQuat Swing;
	FQuat Twist;
	RelativeRotation.ToSwingTwist(FVector::XAxisVector, Swing, Twist);

	// Limit swing.

	Swing = SwingLimitOffset * Swing;

	// Clamp a point with Swing.Y and Swing.Z coordinates to an ellipse with LimitsSettings.Swing2Limit
	// and LimitsSettings.Swing1Limit dimensions. A simplified and not very accurate algorithm is used here,
	// but it is enough for our needs. To get a more accurate result, you can use an algorithm similar
	// to the one used in Chaos::NearPointOnEllipse() or FRigUnit_SphericalPoseReader::DistanceToEllipse(),
	// or https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Geometry/Ellipse.h#L30.

	const auto Swing1Limit{FMath::Sin(FMath::DegreesToRadians(Swing1LimitAngle) * 0.5f)};
	const auto Swing2Limit{FMath::Sin(FMath::DegreesToRadians(Swing2LimitAngle) * 0.5f)};

	FVector2D SwingLimit{Swing.Y, Swing.Z};
	SwingLimit.Normalize();

	SwingLimit.X = FMath::Abs(SwingLimit.X * Swing2Limit);
	SwingLimit.Y = FMath::Abs(SwingLimit.Y * Swing1Limit);

	const auto NewSwingY{FMath::Sign(Swing.Y) * FMath::Min(FMath::Abs(Swing.Y), SwingLimit.X)};
	const auto NewSwingZ{FMath::Sign(Swing.Z) * FMath::Min(FMath::Abs(Swing.Z), SwingLimit.Y)};

	FQuat NewSwing{0.0f, NewSwingY, NewSwingZ, FMath::Sqrt(FMath::Max(0.0f, 1.0f - FMath::Square(NewSwingY) - FMath::Square(NewSwingZ)))};

	NewSwing = SwingLimitOffset.Inverse() * NewSwing;

	// Limit twist.

	const auto TwistLimit{FMath::Sin(FMath::DegreesToRadians(TwistLimitAngle) * 0.5f)};

	const auto NewTwistX{FMath::Sign(Twist.X) * FMath::Min(FMath::Abs(Twist.X), TwistLimit)};

	const FQuat NewTwist(NewTwistX, 0.0f, 0.0f, FMath::Sqrt(FMath::Max(0.0f, 1.0f - FMath::Square(NewTwistX))));

	const auto NewRelativeRotation{NewSwing * NewTwist};

	ConstrainedRotation = CurrentRotation * NewRelativeRotation;
	ConstrainedRotation.Normalize();
}
