#include "Utility/AlsMath.h"

#include "DrawDebugHelpers.h"

namespace SpringInterpolationConstants
{
	static constexpr float MaxDeltaTime{0.1f};
	static constexpr float SubstepDeltaTime{1.0f / 60.0f};
}

float UAlsMath::InterpolateFloatSpringStable(const float Current, const float Target, FFloatSpringState& SpringState,
                                             const float Stiffness, const float CriticalDampingFactor, float DeltaTime, const float Mass)
{
	// Clamp to avoid large delta time.
	DeltaTime = FMath::Min(DeltaTime, SpringInterpolationConstants::MaxDeltaTime);

	auto Result{Current};
	auto PreviousSubstepTime{0.0f};

	for (auto SubstepNumber{1};; SubstepNumber++)
	{
		const auto SubstepTime{SubstepNumber * SpringInterpolationConstants::SubstepDeltaTime};
		if (SubstepTime < DeltaTime - SMALL_NUMBER)
		{
			Result = UKismetMathLibrary::FloatSpringInterp(Result, Target, SpringState, Stiffness, CriticalDampingFactor,
			                                               SpringInterpolationConstants::SubstepDeltaTime, Mass);

			PreviousSubstepTime = SubstepTime;
		}
		else
		{
			return UKismetMathLibrary::FloatSpringInterp(Result, Target, SpringState, Stiffness, CriticalDampingFactor,
			                                             DeltaTime - PreviousSubstepTime, Mass);
		}
	}
}

FVector UAlsMath::InterpolateVectorSpringStable(const FVector& Current, const FVector& Target, FVectorSpringState& SpringState,
                                                const float Stiffness, const float CriticalDampingFactor, float DeltaTime, const float Mass)
{
	// Clamp to avoid large delta time.
	DeltaTime = FMath::Min(DeltaTime, SpringInterpolationConstants::MaxDeltaTime);

	auto Result{Current};
	auto PreviousSubstepTime{0.0f};

	for (auto SubstepNumber{1};; SubstepNumber++)
	{
		const auto SubstepTime{SubstepNumber * SpringInterpolationConstants::SubstepDeltaTime};
		if (SubstepTime < DeltaTime - SMALL_NUMBER)
		{
			Result = UKismetMathLibrary::VectorSpringInterp(Result, Target, SpringState, Stiffness, CriticalDampingFactor,
			                                                SpringInterpolationConstants::SubstepDeltaTime, Mass);

			PreviousSubstepTime = SubstepTime;
		}
		else
		{
			return UKismetMathLibrary::VectorSpringInterp(Result, Target, SpringState, Stiffness, CriticalDampingFactor,
			                                              DeltaTime - PreviousSubstepTime, Mass);
		}
	}
}

FVector UAlsMath::SlerpSkipNormalization(const FVector& From, const FVector& To, const float Alpha)
{
	// http://allenchou.net/2018/05/game-math-deriving-the-slerp-formula/

	const auto Dot{From | To};

	if (Dot > 0.9995f || Dot < -0.9995f)
	{
		return FMath::Lerp(From, To, Alpha).GetSafeNormal();
	}

	const auto Theta{FMath::Acos(Dot) * Alpha};
	const auto FromPerpendicular{(To - From * Dot).GetSafeNormal()};

	float Sin;
	float Cos;
	FMath::SinCos(&Sin, &Cos, Theta);

	return From * Cos + FromPerpendicular * Sin;
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
