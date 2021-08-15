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
	static constexpr float RadToDeg{180 / PI};
	static constexpr float DegToRad{PI / 180};

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float Clamp01(float Value);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float LerpClamped(float A, float B, float Alpha);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float AngleInterpolateConstantTo(const float CurrentAngle, float const TargetAngle, const float DeltaTime,
	                                        const float InterpolationSpeed);

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
	if (Value <= 0)
	{
		return 0;
	}

	if (Value >= 1)
	{
		return 1;
	}

	return Value;
}

inline float UAlsMath::LerpClamped(const float A, const float B, const float Alpha)
{
	return A + Clamp01(Alpha) * (B - A);
}

inline float UAlsMath::AngleInterpolateConstantTo(const float CurrentAngle, float const TargetAngle,
                                                  const float DeltaTime, const float InterpolationSpeed)
{
	if (DeltaTime <= 0 || CurrentAngle == TargetAngle)
	{
		return CurrentAngle;
	}

	if (InterpolationSpeed <= 0)
	{
		// If no interpolation speed, then jump to target value.
		return TargetAngle;
	}

	const auto Step{InterpolationSpeed * DeltaTime};
	return FRotator::NormalizeAxis(CurrentAngle + FMath::Clamp(FRotator::NormalizeAxis(TargetAngle - CurrentAngle), -Step, Step));
}

inline FVector UAlsMath::ClampMagnitude01(const FVector& Vector)
{
	const auto MagnitudeSquared{Vector.SizeSquared()};
	if (MagnitudeSquared <= 1)
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
	return RadianToDirection(Angle * DegToRad);
}

inline float UAlsMath::DirectionToAngle(const FVector2D& Direction)
{
	return FMath::Atan2(Direction.Y, Direction.X) * RadToDeg;
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
