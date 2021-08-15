#include "Utility/AlsMath.h"

#include "DrawDebugHelpers.h"

float UAlsMath::FixGamepadDiagonalValues(const float AxisValue, const float OtherAxisValue)
{
	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Format(TEXT("Vale: {0}, OtherAxis: {1} Result: {2}"), {
	// 	                                                                     Value, OtherAxis, FMath::Clamp(AxisValue *
	// 			                                                                     FMath::GetMappedRangeValueClamped({0, 0.6f},
	// 				                                                                     {1, 1.2f},
	// 				                                                                     FMath::Abs(OtherAxisValue)),
	// 		                                                                     -1.0f, 1.0f)
	//                                                                      }));

	return FMath::Clamp(AxisValue *
	                    FMath::GetMappedRangeValueClamped({0, 0.6f},
	                                                      {1, 1.2f},
	                                                      FMath::Abs(OtherAxisValue)),
	                    -1.0f, 1.0f);
}

EAlsMovementDirection UAlsMath::CalculateMovementDirection(const float Angle, const float ForwardHalfAngle, const float AngleOffset)
{
	if (Angle >= -ForwardHalfAngle - AngleOffset && Angle <= ForwardHalfAngle + AngleOffset)
	{
		return EAlsMovementDirection::Forward;
	}

	if (Angle >= ForwardHalfAngle - AngleOffset && Angle <= 180 - ForwardHalfAngle + AngleOffset)
	{
		return EAlsMovementDirection::Right;
	}

	if (Angle <= -(ForwardHalfAngle - AngleOffset) && Angle >= -(180 - ForwardHalfAngle + AngleOffset))
	{
		return EAlsMovementDirection::Left;
	}

	return EAlsMovementDirection::Backward;
}
