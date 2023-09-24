#include "Utility/AlsMath.h"

#include "State/AlsMovementDirection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsMath)

float UAlsMath::SpringDampFloat(const float Current, const float Target, FAlsSpringFloatState& SpringState, const float DeltaTime,
                                const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	return SpringDamp(Current, Target, SpringState, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

FVector UAlsMath::SpringDampVector(const FVector& Current, const FVector& Target, FAlsSpringVectorState& SpringState, const float DeltaTime,
                                   const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	return SpringDamp(Current, Target, SpringState, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

FVector UAlsMath::SlerpSkipNormalization(const FVector& From, const FVector& To, const float Alpha)
{
	// http://allenchou.net/2018/05/game-math-deriving-the-slerp-formula/

	const auto Dot{From | To};

	if (Dot > 0.9995f || Dot < -0.9995f)
	{
		return FMath::Lerp(From, To, Alpha).GetSafeNormal();
	}

	const auto Theta{UE_REAL_TO_FLOAT(FMath::Acos(Dot)) * Alpha};

	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Theta);

	const auto FromPerpendicular{(To - From * Dot).GetSafeNormal()};

	return From * Cos + FromPerpendicular * Sin;
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
                                      FVector& ProjectionLocation, FVector& Direction)
{
	const auto AbVector{BLocation - ALocation};
	if (AbVector.IsNearlyZero())
	{
		// Can't do anything if A and B are equal.

		ProjectionLocation = ALocation;
		Direction = FVector::ZeroVector;

		return false;
	}

	auto AcVector{CLocation - ALocation};
	if (!AcVector.Normalize())
	{
		// Only A and C are equal.

		ProjectionLocation = ALocation;
		Direction = AbVector.GetUnsafeNormal(); // A and B are not equal, so normalization will be safe.

		return true;
	}

	ProjectionLocation = ALocation + AbVector.ProjectOnToNormal(AcVector);
	Direction = BLocation - ProjectionLocation;

	return Direction.Normalize(); // Direction will be zero and cannot be normalized if A, B and C are collinear.
}
