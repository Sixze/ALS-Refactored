#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "State/Enumerations/AlsMovementDirection.h"

#include "AlsMath.generated.h"

class UCapsuleComponent;

UCLASS()
class ALS_API UAlsMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float Clamp01(float Value);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float LerpClamped(float A, float B, float Alpha);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float InterpolateAngleConstant(float CurrentAngle, float TargetAngle, float DeltaTime, float InterpolationSpeed);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector ClampMagnitude01(const FVector& Vector);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector2D RadianToDirection(float Radian);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector2D AngleToDirection(float Angle);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static float DirectionToAngle(const FVector2D& Direction);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Math|Transform")
	static FTransform AddTransforms(const FTransform& Left, const FTransform& Right);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Math|Transform")
	static FTransform SubtractTransforms(const FTransform& Left, const FTransform& Right);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Math|Input")
	static float FixGamepadDiagonalValues(float AxisValue, float OtherAxisValue);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Math|Input")
	static EAlsMovementDirection CalculateMovementDirection(float Angle, float ForwardHalfAngle, float AngleOffset);
};

inline float UAlsMath::Clamp01(const float Value)
{
	return Value <= 0.0f ? 0.0f : Value >= 1.0f ? 1.0f : Value;
}

inline float UAlsMath::LerpClamped(const float A, const float B, const float Alpha)
{
	return A + Clamp01(Alpha) * (B - A);
}

inline FVector UAlsMath::ClampMagnitude01(const FVector& Vector)
{
	const auto MagnitudeSquared{Vector.SizeSquared()};
	if (MagnitudeSquared <= 1.0f)
	{
		return Vector;
	}

	const auto Scale{FMath::InvSqrt(MagnitudeSquared)};
	return {Vector.X * Scale, Vector.Y * Scale, Vector.Z * Scale};
}

inline FVector2D UAlsMath::RadianToDirection(const float Radian)
{
	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Radian);

	return {Cos, Sin};
}

inline FVector2D UAlsMath::AngleToDirection(const float Angle)
{
	return RadianToDirection(FMath::DegreesToRadians(Angle));
}

inline float UAlsMath::DirectionToAngle(const FVector2D& Direction)
{
	return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline FTransform UAlsMath::AddTransforms(const FTransform& Left, const FTransform& Right)
{
	return {
		Left.Rotator() + Right.Rotator(),
		Left.GetLocation() + Right.GetLocation(),
		Left.GetScale3D() + Right.GetScale3D()
	};
}

inline FTransform UAlsMath::SubtractTransforms(const FTransform& Left, const FTransform& Right)
{
	return {
		Left.Rotator() - Right.Rotator(),
		Left.GetLocation() - Right.GetLocation(),
		Left.GetScale3D() - Right.GetScale3D()
	};
}
