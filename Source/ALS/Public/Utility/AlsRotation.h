#pragma once

#include "AlsMath.h"
#include "AlsRotation.generated.h"

UCLASS()
class ALS_API UAlsRotation : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr auto CounterClockwiseRotationAngleThreshold{5.0f};

public:
	// Remaps the angle from the [175, 180] range to [-185, -180]. Used to
	// make the character rotate counterclockwise during a 180 degree turn.
	template <typename ValueType> requires std::is_floating_point_v<ValueType>
	static constexpr ValueType RemapAngleForCounterClockwiseRotation(ValueType Angle);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float LerpAngle(float From, float To, float Ratio);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Rotation"))
	static FRotator LerpRotation(const FRotator& From, const FRotator& To, float Ratio);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float InterpolateAngleConstant(float Current, float Target, float DeltaTime, float Speed);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float DampAngle(float Current, float Target, float DeltaTime, float Smoothing);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility",
		Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Rotation"))
	static FRotator DampRotation(const FRotator& Current, const FRotator& Target, float DeltaTime, float Smoothing);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float ExponentialDecayAngle(float Current, float Target, float DeltaTime, float Lambda);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility",
		Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Rotation"))
	static FRotator ExponentialDecayRotation(const FRotator& Current, const FRotator& Target, float DeltaTime, float Lambda);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (AutoCreateRefTerm = "TwistAxis", ReturnDisplayName = "Twist"))
	static FQuat GetTwist(const FQuat& Quaternion, const FVector& TwistAxis = FVector::UpVector);
};

template <typename ValueType> requires std::is_floating_point_v<ValueType>
constexpr ValueType UAlsRotation::RemapAngleForCounterClockwiseRotation(const ValueType Angle)
{
	if (Angle > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		return Angle - 360.0f;
	}

	return Angle;
}

inline float UAlsRotation::LerpAngle(const float From, const float To, const float Ratio)
{
	auto Delta{FRotator3f::NormalizeAxis(To - From)};
	Delta = RemapAngleForCounterClockwiseRotation(Delta);

	return FRotator3f::NormalizeAxis(From + Delta * Ratio);
}

inline FRotator UAlsRotation::LerpRotation(const FRotator& From, const FRotator& To, const float Ratio)
{
	auto Result{To - From};
	Result.Normalize();

	Result.Pitch = RemapAngleForCounterClockwiseRotation(Result.Pitch);
	Result.Yaw = RemapAngleForCounterClockwiseRotation(Result.Yaw);
	Result.Roll = RemapAngleForCounterClockwiseRotation(Result.Roll);

	Result *= Ratio;
	Result += From;
	Result.Normalize();

	return Result;
}

inline float UAlsRotation::InterpolateAngleConstant(const float Current, const float Target, const float DeltaTime, const float Speed)
{
	if (Speed <= 0.0f || Current == Target)
	{
		return Target;
	}

	auto Delta{FRotator3f::NormalizeAxis(Target - Current)};
	Delta = RemapAngleForCounterClockwiseRotation(Delta);

	const auto MaxDelta{Speed * DeltaTime};

	return FRotator3f::NormalizeAxis(Current + FMath::Clamp(Delta, -MaxDelta, MaxDelta));
}

inline float UAlsRotation::DampAngle(const float Current, const float Target, const float DeltaTime, const float Smoothing)
{
	return Smoothing > 0.0f
		       ? LerpAngle(Current, Target, UAlsMath::Damp(DeltaTime, Smoothing))
		       : Target;
}

inline float UAlsRotation::ExponentialDecayAngle(const float Current, const float Target, const float DeltaTime, const float Lambda)
{
	return Lambda > 0.0f
		       ? LerpAngle(Current, Target, UAlsMath::ExponentialDecay(DeltaTime, Lambda))
		       : Target;
}

inline FRotator UAlsRotation::DampRotation(const FRotator& Current, const FRotator& Target, const float DeltaTime, const float Smoothing)
{
	return Smoothing > 0.0f
		       ? LerpRotation(Current, Target, UAlsMath::Damp(DeltaTime, Smoothing))
		       : Target;
}

inline FRotator UAlsRotation::ExponentialDecayRotation(const FRotator& Current, const FRotator& Target,
                                                       const float DeltaTime, const float Lambda)
{
	return Lambda > 0.0f
		       ? LerpRotation(Current, Target, UAlsMath::ExponentialDecay(DeltaTime, Lambda))
		       : Target;
}

inline FQuat UAlsRotation::GetTwist(const FQuat& Quaternion, const FVector& TwistAxis)
{
	// Based on TQuat<T>::ToSwingTwist().

	const auto Projection{(TwistAxis | FVector{Quaternion.X, Quaternion.Y, Quaternion.Z}) * TwistAxis};

	return FQuat{Projection.X, Projection.Y, Projection.Z, Quaternion.W}.GetNormalized();
}
