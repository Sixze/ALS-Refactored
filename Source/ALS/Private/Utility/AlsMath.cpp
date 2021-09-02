#include "Utility/AlsMath.h"

#include "DrawDebugHelpers.h"

float UAlsMath::InterpolateAngleConstant(const float CurrentAngle, const float TargetAngle,
                                         const float DeltaTime, const float InterpolationSpeed)
{
	if (DeltaTime <= 0.0f || CurrentAngle == TargetAngle)
	{
		return CurrentAngle;
	}

	if (InterpolationSpeed <= 0.0f)
	{
		// If no interpolation speed, then jump to target value.
		return TargetAngle;
	}

	const auto Step{InterpolationSpeed * DeltaTime};
	return FRotator::NormalizeAxis(CurrentAngle + FMath::Clamp(FRotator::NormalizeAxis(TargetAngle - CurrentAngle), -Step, Step));
}

FVector UAlsMath::SlerpSkipNormalization(const FVector& From, const FVector& To, const float Alpha)
{
	const auto Dot{From | To};

	if (Dot > 0.9995f || Dot < -0.9995f)
	{
		return FMath::Lerp(From, To, Alpha).GetSafeNormal();
	}

	const auto Theta{FMath::Acos(Dot) * Alpha};
	const auto Vector{(To - From * Dot).GetSafeNormal()};

	float Sin;
	float Cos;
	FMath::SinCos(&Sin, &Cos, Theta);

	return From * Cos + Vector * Sin;
}

float UAlsMath::FixGamepadDiagonalValues(const float AxisValue, const float OtherAxisValue)
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red,
	//                                  FString::Format(TEXT("Value: {0}, OtherAxis: {1} Result: {2}"), {
	// 	                                                 AxisValue, OtherAxisValue, FMath::Clamp(AxisValue *
	// 			                                                 FMath::GetMappedRangeValueClamped({0.0f, 0.6f},
	// 				                                                 {1.0f, 1.2f},
	// 				                                                 FMath::Abs(OtherAxisValue)),
	// 		                                                 -1.0f, 1.0f)
	//                                                  }));

	return FMath::Clamp(AxisValue *
	                    FMath::GetMappedRangeValueClamped({0.0f, 0.6f},
	                                                      {1.0f, 1.2f},
	                                                      FMath::Abs(OtherAxisValue)),
	                    -1.0f, 1.0f);
}

EAlsMovementDirection UAlsMath::CalculateMovementDirection(const float Angle, const float ForwardHalfAngle, const float AngleOffset)
{
	if (Angle >= -ForwardHalfAngle - AngleOffset && Angle <= ForwardHalfAngle + AngleOffset)
	{
		return EAlsMovementDirection::Forward;
	}

	if (Angle >= ForwardHalfAngle - AngleOffset && Angle <= 180.0f - ForwardHalfAngle + AngleOffset)
	{
		return EAlsMovementDirection::Right;
	}

	if (Angle <= -(ForwardHalfAngle - AngleOffset) && Angle >= -(180.0f - ForwardHalfAngle + AngleOffset))
	{
		return EAlsMovementDirection::Left;
	}

	return EAlsMovementDirection::Backward;
}
