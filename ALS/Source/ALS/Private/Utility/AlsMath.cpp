#include "Utility/AlsMath.h"

#include "DrawDebugHelpers.h"

float UAlsMath::FixGamepadDiagonalValues(const float AxisValue, const float OtherAxisValue)
{
	// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Format(TEXT("Value: {0}, OtherAxis: {1} Result: {2}"), {
	// 	                                                                        AxisValue, OtherAxisValue, FMath::Clamp(AxisValue *
	// 			                                                                        FMath::GetMappedRangeValueClamped({0.0f, 0.6f},
	// 				                                                                        {1.0f, 1.2f},
	// 				                                                                        FMath::Abs(OtherAxisValue)),
	// 		                                                                        -1.0f, 1.0f)
	//                                                                         }));

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
