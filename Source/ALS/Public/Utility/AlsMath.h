#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "State/Enumerations/AlsMovementDirection.h"

#include "AlsMath.generated.h"

class UCapsuleComponent;

UCLASS()
class ALS_API UAlsMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr float TwoPi{6.2831853071795864769252867665590057683943387987502116419498891846f};
	static constexpr float CounterClockwiseRotationAngleThreshold{5.0f};

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float Clamp01(float Value);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float LerpClamped(float A, float B, float Alpha);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float LerpAngle(float A, float B, float Alpha);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math", Meta = (AutoCreateRefTerm = "A, B"))
	static FRotator LerpRotator(const FRotator& A, const FRotator& B, float Alpha);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float Damp(float DeltaTime, float Smoothing);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float ExponentialDecay(float DeltaTime, float Lambda);

	template <class T>
	static T Damp(const T& Current, const T& Target, float DeltaTime, float Smoothing);

	template <class T>
	static T ExponentialDecay(const T& Current, const T& Target, float DeltaTime, float Lambda);

	static FRotator Damp(const FRotator& Current, const FRotator& Target, float DeltaTime, float Smoothing);

	static FRotator ExponentialDecay(const FRotator& Current, const FRotator& Target, float DeltaTime, float Lambda);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float DampAngle(float Current, float Target, float DeltaTime, float Smoothing);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float ExponentialDecayAngle(float Current, float Target, float DeltaTime, float Lambda);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math", Meta = (AutoCreateRefTerm = "Current, Target"))
	static FRotator DampRotator(const FRotator& Current, const FRotator& Target, float DeltaTime, float Smoothing);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math", Meta = (AutoCreateRefTerm = "Current, Target"))
	static FRotator ExponentialDecayRotator(const FRotator& Current, const FRotator& Target, float DeltaTime, float Lambda);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math")
	static float InterpolateAngleConstant(float Current, float Target, float DeltaTime, float InterpolationSpeed);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Math")
	static float InterpolateFloatSpringStable(float Current, float Target, UPARAM(ref) FFloatSpringState& SpringState,
	                                          float Stiffness, float CriticalDampingFactor, float DeltaTime, float Mass = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Math", Meta = (AutoCreateRefTerm = "Current, Target"))
	static FVector InterpolateVectorSpringStable(const FVector& Current, const FVector& Target, UPARAM(ref) FVectorSpringState& SpringState,
	                                             float Stiffness, float CriticalDampingFactor, float DeltaTime, float Mass = 1.0f);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector", Meta = (AutoCreateRefTerm = "Vector"))
	static FVector ClampMagnitude01(const FVector& Vector);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector2D RadianToDirection(float Radian);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector RadianToDirection3D(float Radian);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector2D AngleToDirection(float Angle);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector")
	static FVector AngleToDirection2D(float Angle);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector", Meta = (AutoCreateRefTerm = "Direction"))
	static float DirectionToAngle(const FVector2D& Direction);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector", Meta = (AutoCreateRefTerm = "Direction"))
	static float DirectionToAngle2D(const FVector& Direction);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector",
		DisplayName = "Angle Between (Skip Normalization)", Meta = (AutoCreateRefTerm = "From, To"))
	static float AngleBetweenSkipNormalization(const FVector& From, const FVector& To);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Math|Vector", DisplayName = "Slerp (Skip Normalization)")
	static FVector SlerpSkipNormalization(const FVector& From, const FVector& To, float Alpha);

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
	return A + (B - A) * Clamp01(Alpha);
}

inline float UAlsMath::LerpAngle(const float A, const float B, const float Alpha)
{
	auto Delta{FRotator::NormalizeAxis(B - A)};

	if (Delta > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		Delta -= 360.0f;
	}

	return FRotator::NormalizeAxis(A + Delta * Alpha);
}

inline FRotator UAlsMath::LerpRotator(const FRotator& A, const FRotator& B, const float Alpha)
{
	auto Result{B - A};
	Result.Normalize();

	if (Result.Pitch > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		Result.Pitch -= 360.0f;
	}

	if (Result.Yaw > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		Result.Yaw -= 360.0f;
	}

	if (Result.Roll > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		Result.Roll -= 360.0f;
	}

	Result *= Alpha;
	Result += A;
	Result.Normalize();

	return Result;
}

inline float UAlsMath::Damp(const float DeltaTime, const float Smoothing)
{
	// https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

	return 1.0f - FMath::Pow(Smoothing, DeltaTime);
}

inline float UAlsMath::ExponentialDecay(const float DeltaTime, const float Lambda)
{
	// https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

	return 1.0f - FMath::Exp(-Lambda * DeltaTime);
}

template <class T>
T UAlsMath::Damp(const T& Current, const T& Target, const float DeltaTime, const float Smoothing)
{
	return Smoothing <= 0.0f ? Target : FMath::Lerp(Current, Target, Damp(DeltaTime, Smoothing));
}

template <class T>
T UAlsMath::ExponentialDecay(const T& Current, const T& Target, const float DeltaTime, const float Lambda)
{
	return Lambda <= 0.0f ? Target : FMath::Lerp(Current, Target, ExponentialDecay(DeltaTime, Lambda));
}

inline FRotator UAlsMath::Damp(const FRotator& Current, const FRotator& Target, const float DeltaTime, const float Smoothing)
{
	return Smoothing <= 0.0f ? Target : LerpRotator(Current, Target, Damp(DeltaTime, Smoothing));
}

inline FRotator UAlsMath::ExponentialDecay(const FRotator& Current, const FRotator& Target, const float DeltaTime, const float Lambda)
{
	return Lambda <= 0.0f ? Target : LerpRotator(Current, Target, ExponentialDecay(DeltaTime, Lambda));
}

inline float UAlsMath::DampAngle(const float Current, const float Target, const float DeltaTime, const float Smoothing)
{
	return Smoothing <= 0.0f ? Target : LerpAngle(Current, Target, Damp(DeltaTime, Smoothing));
}

inline float UAlsMath::ExponentialDecayAngle(const float Current, const float Target, const float DeltaTime, const float Lambda)
{
	return Lambda <= 0.0f ? Target : LerpAngle(Current, Target, ExponentialDecay(DeltaTime, Lambda));
}

inline FRotator UAlsMath::DampRotator(const FRotator& Current, const FRotator& Target, const float DeltaTime, const float Smoothing)
{
	return Damp(Current, Target, DeltaTime, Smoothing);
}

inline FRotator UAlsMath::ExponentialDecayRotator(const FRotator& Current, const FRotator& Target,
                                                  const float DeltaTime, const float Lambda)
{
	return ExponentialDecay(Current, Target, DeltaTime, Lambda);
}

inline float UAlsMath::InterpolateAngleConstant(const float Current, const float Target,
                                                const float DeltaTime, const float InterpolationSpeed)
{
	if (InterpolationSpeed <= 0.0f || Current == Target)
	{
		return Target;
	}

	auto Delta{FRotator::NormalizeAxis(Target - Current)};

	if (Delta > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		Delta -= 360.0f;
	}

	const auto Alpha{InterpolationSpeed * DeltaTime};

	return FRotator::NormalizeAxis(Current + FMath::Clamp(Delta, -Alpha, Alpha));
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

inline FVector UAlsMath::RadianToDirection3D(const float Radian)
{
	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Radian);

	return {Cos, Sin, 0.0f};
}

inline FVector2D UAlsMath::AngleToDirection(const float Angle)
{
	return RadianToDirection(FMath::DegreesToRadians(Angle));
}

inline FVector UAlsMath::AngleToDirection2D(const float Angle)
{
	return RadianToDirection3D(FMath::DegreesToRadians(Angle));
}

inline float UAlsMath::DirectionToAngle(const FVector2D& Direction)
{
	return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline float UAlsMath::DirectionToAngle2D(const FVector& Direction)
{
	return FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
}

inline float UAlsMath::AngleBetweenSkipNormalization(const FVector& From, const FVector& To)
{
	return FMath::RadiansToDegrees(FMath::Acos(From | To));
}
