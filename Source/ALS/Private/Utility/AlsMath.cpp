#include "Utility/AlsMath.h"

#include "State/AlsMovementDirection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsMath)

float UAlsMath::SpringDampFloat(FAlsSpringFloatState& SpringState, const float Current, const float Target, const float DeltaTime,
                                const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	return SpringDamp(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

FVector UAlsMath::SpringDampVector(FAlsSpringVectorState& SpringState, const FVector& Current, const FVector& Target, const float DeltaTime,
                                   const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	return SpringDamp(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}

FVector UAlsMath::SlerpSkipNormalization(const FVector& From, const FVector& To, const float Ratio)
{
	// https://allenchou.net/2018/05/game-math-deriving-the-slerp-formula/

	const auto Dot{From | To};

	if (Dot > 0.9995f || Dot < -0.9995f)
	{
		return FMath::Lerp(From, To, Ratio).GetSafeNormal();
	}

	const auto Theta{UE_REAL_TO_FLOAT(FMath::Acos(Dot)) * Ratio};

	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Theta);

	const auto FromPerpendicular{(To - From * Dot).GetSafeNormal()};

	return From * Cos + FromPerpendicular * Sin;
}

FQuat UAlsMath::GetTwist(const FQuat& Quaternion, const FVector& TwistAxis)
{
	// Based on TQuat<T>::ToSwingTwist().

	const auto Projection{(TwistAxis | FVector{Quaternion.X, Quaternion.Y, Quaternion.Z}) * TwistAxis};

	return FQuat{Projection.X, Projection.Y, Projection.Z, Quaternion.W}.GetNormalized();
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
