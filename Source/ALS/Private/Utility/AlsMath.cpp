#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsMath)

float UAlsMath::SpringDamperFloat(FAlsSpringFloatState& SpringState, const float Current, const float Target, const float DeltaTime,
                                  const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	return SpringDamper(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

EAlsMovementDirection UAlsMath::CalculateMovementDirection(const float Angle, const float ForwardHalfAngle, const float AngleThreshold)
{
	if (Angle >= -ForwardHalfAngle - AngleThreshold && Angle <= ForwardHalfAngle + AngleThreshold)
	{
		return EAlsMovementDirection::Forward;
	}

	if (Angle >= ForwardHalfAngle - AngleThreshold && Angle <= 180.0f - ForwardHalfAngle + AngleThreshold)
	{
		return EAlsMovementDirection::Right;
	}

	if (Angle <= -(ForwardHalfAngle - AngleThreshold) && Angle >= -(180.0f - ForwardHalfAngle + AngleThreshold))
	{
		return EAlsMovementDirection::Left;
	}

	return EAlsMovementDirection::Backward;
}

bool UAlsMath::TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation, const FVector& CLocation,
                                      FVector& ProjectionLocation, FVector& PoleDirection)
{
	const auto AbVector{BLocation - ALocation};
	if (AbVector.IsNearlyZero())
	{
		// Can't do anything if A and B are equal.

		ProjectionLocation = ALocation;
		PoleDirection = FVector::ZeroVector;

		return false;
	}

	auto AcVector{CLocation - ALocation};
	if (!AcVector.Normalize())
	{
		// Only A and C are equal.

		ProjectionLocation = ALocation;
		PoleDirection = AbVector.GetUnsafeNormal(); // A and B are not equal, so normalization will be safe.

		return true;
	}

	ProjectionLocation = ALocation + AbVector.ProjectOnToNormal(AcVector);
	PoleDirection = BLocation - ProjectionLocation;

	return PoleDirection.Normalize(); // Direction will be zero and cannot be normalized if A, B and C are collinear.
}
