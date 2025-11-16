#pragma once

#include "AlsMath.h"
#include "AlsRotation.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALS_API UAlsRotation : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr auto CounterClockwiseRotationAngleThreshold{5.0f};

public:
	template <typename ValueType> requires UE::CFloatingPoint<ValueType>
	static constexpr ValueType RemapAngleForCounterClockwiseRotation(ValueType Angle);

	static VectorRegister4Double RemapRotationForCounterClockwiseRotation(const VectorRegister4Double& Rotation);

	// Remaps the angle from the [175, 180] range to [-185, -180]. Used to
	// make the character rotate counterclockwise during a 180 degree turn.
	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float RemapAngleForCounterClockwiseRotation(float Angle);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float LerpAngle(float From, float To, float Ratio);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (AutoCreateRefTerm = "From, To", ReturnDisplayName = "Rotation"))
	static FRotator LerpRotation(const FRotator& From, const FRotator& To, float Ratio);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float InterpolateAngleConstant(float Current, float Target, float DeltaTime, float Speed);

	// HalfLife is the time it takes for the distance to the target to be reduced by half.
	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Angle"))
	static float DamperExactAngle(float Current, float Target, float DeltaTime, float HalfLife);

	// HalfLife is the time it takes for the distance to the target to be reduced by half.
	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility",
		Meta = (AutoCreateRefTerm = "Current, Target", ReturnDisplayName = "Rotation"))
	static FRotator DamperExactRotation(const FRotator& Current, const FRotator& Target, float DeltaTime, float HalfLife);

	// Same as FMath::QInterpTo(), but uses FQuat::FastLerp() instead of FQuat::Slerp().
	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (ReturnDisplayName = "Quaternion"))
	static FQuat InterpolateQuaternionFast(const FQuat& Current, const FQuat& Target, float DeltaTime, float Speed);

	UFUNCTION(BlueprintPure, Category = "ALS|Rotation Utility", Meta = (AutoCreateRefTerm = "TwistAxis", ReturnDisplayName = "Twist"))
	static FQuat GetTwist(const FQuat& Quaternion, const FVector& TwistAxis = FVector::UpVector);
};

template <typename ValueType> requires UE::CFloatingPoint<ValueType>
constexpr ValueType UAlsRotation::RemapAngleForCounterClockwiseRotation(const ValueType Angle)
{
	if (Angle > 180.0f - CounterClockwiseRotationAngleThreshold)
	{
		return Angle - 360.0f;
	}

	return Angle;
}

inline VectorRegister4Double UAlsRotation::RemapRotationForCounterClockwiseRotation(const VectorRegister4Double& Rotation)
{
	static constexpr auto RemapThreshold{
		MakeVectorRegisterDoubleConstant(180.0f - CounterClockwiseRotationAngleThreshold, 180.0f - CounterClockwiseRotationAngleThreshold,
		                                 180.0f - CounterClockwiseRotationAngleThreshold, 180.0f - CounterClockwiseRotationAngleThreshold)
	};

	static constexpr auto RemapAngles{MakeVectorRegisterDoubleConstant(360.0f, 360.0f, 360.0f, 0.0f)};

	const auto ReverseRotationMask{VectorCompareGE(Rotation, RemapThreshold)};

	const auto ReversedRotation{VectorSubtract(Rotation, RemapAngles)};

	return VectorSelect(ReverseRotationMask, ReversedRotation, Rotation);
}

inline float UAlsRotation::RemapAngleForCounterClockwiseRotation(const float Angle)
{
	return RemapAngleForCounterClockwiseRotation<float>(Angle);
}

inline float UAlsRotation::LerpAngle(const float From, const float To, const float Ratio)
{
	auto Delta{FMath::UnwindDegrees(To - From)};
	Delta = RemapAngleForCounterClockwiseRotation(Delta);

	return FMath::UnwindDegrees(From + Delta * Ratio);
}

inline FRotator UAlsRotation::LerpRotation(const FRotator& From, const FRotator& To, const float Ratio)
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const auto FromRegister{VectorLoadFloat3_W0(&From)};
	const auto ToRegister{VectorLoadFloat3_W0(&To)};

	auto Delta{VectorSubtract(ToRegister, FromRegister)};
	Delta = VectorNormalizeRotator(Delta);

	if (!VectorAnyGreaterThan(VectorAbs(Delta), GlobalVectorConstants::DoubleKindaSmallNumber))
	{
		return To;
	}

	Delta = RemapRotationForCounterClockwiseRotation(Delta);

	auto ResultRegister{VectorMultiplyAdd(Delta, VectorLoadFloat1(&Ratio), FromRegister)};
	ResultRegister = VectorNormalizeRotator(ResultRegister);

	FRotator Result;
	VectorStoreFloat3(ResultRegister, &Result);

	return Result;
#else
	auto Result{To - From};
	Result.Normalize();

	Result.Pitch = RemapAngleForCounterClockwiseRotation(Result.Pitch);
	Result.Yaw = RemapAngleForCounterClockwiseRotation(Result.Yaw);
	Result.Roll = RemapAngleForCounterClockwiseRotation(Result.Roll);

	Result *= Ratio;
	Result += From;
	Result.Normalize();

	return Result;
#endif
}

inline float UAlsRotation::InterpolateAngleConstant(const float Current, const float Target, const float DeltaTime, const float Speed)
{
	auto Delta{FMath::UnwindDegrees(Target - Current)};
	const auto MaxDelta{Speed * DeltaTime};

	if (Speed <= 0.0f || FMath::Abs(Delta) <= MaxDelta)
	{
		return Target;
	}

	Delta = RemapAngleForCounterClockwiseRotation(Delta);
	return FMath::UnwindDegrees(Current + FMath::Sign(Delta) * MaxDelta);
}

inline float UAlsRotation::DamperExactAngle(const float Current, const float Target, const float DeltaTime, const float HalfLife)
{
	auto Delta{FMath::UnwindDegrees(Target - Current)};

	if (FMath::IsNearlyZero(Delta, UE_KINDA_SMALL_NUMBER))
	{
		return Target;
	}

	Delta = RemapAngleForCounterClockwiseRotation(Delta);

	const auto Alpha{UAlsMath::DamperExactAlpha(DeltaTime, HalfLife)};
	return FMath::UnwindDegrees(Current + Delta * Alpha);
}

inline FRotator UAlsRotation::DamperExactRotation(const FRotator& Current, const FRotator& Target,
                                                  const float DeltaTime, const float HalfLife)
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	const auto CurrentRegister{VectorLoadFloat3_W0(&Current)};
	const auto TargetRegister{VectorLoadFloat3_W0(&Target)};

	auto Delta{VectorSubtract(TargetRegister, CurrentRegister)};
	Delta = VectorNormalizeRotator(Delta);

	if (!VectorAnyGreaterThan(VectorAbs(Delta), GlobalVectorConstants::DoubleKindaSmallNumber))
	{
		return Target;
	}

	Delta = RemapRotationForCounterClockwiseRotation(Delta);

	const double Alpha{UAlsMath::DamperExactAlpha(DeltaTime, HalfLife)};

	auto ResultRegister{VectorMultiplyAdd(Delta, VectorLoadDouble1(&Alpha), CurrentRegister)};
	ResultRegister = VectorNormalizeRotator(ResultRegister);

	FRotator Result;
	VectorStoreFloat3(ResultRegister, &Result);

	return Result;
#else
	auto Result{Target - Current};
	Result.Normalize();

	if (FMath::IsNearlyZero(Result.Pitch, UE_KINDA_SMALL_NUMBER) &&
	    FMath::IsNearlyZero(Result.Yaw, UE_KINDA_SMALL_NUMBER) &&
	    FMath::IsNearlyZero(Result.Roll, UE_KINDA_SMALL_NUMBER))
	{
		return Target;
	}

	Result.Pitch = RemapAngleForCounterClockwiseRotation(Result.Pitch);
	Result.Yaw = RemapAngleForCounterClockwiseRotation(Result.Yaw);
	Result.Roll = RemapAngleForCounterClockwiseRotation(Result.Roll);

	const auto Alpha{UAlsMath::DamperExactAlpha(DeltaTime, HalfLife)};

	Result *= Alpha;
	Result += Current;
	Result.Normalize();

	return Result;
#endif
}

inline FQuat UAlsRotation::InterpolateQuaternionFast(const FQuat& Current, const FQuat& Target, const float DeltaTime, const float Speed)
{
	if (Speed <= 0.0f || Current.Equals(Target))
	{
		return Target;
	}

	return FQuat::FastLerp(Current, Target, UAlsMath::Clamp01(Speed * DeltaTime)).GetNormalized();
}

inline FQuat UAlsRotation::GetTwist(const FQuat& Quaternion, const FVector& TwistAxis)
{
	// Based on TQuat<T>::ToSwingTwist().

	const auto Projection{(TwistAxis | FVector{Quaternion.X, Quaternion.Y, Quaternion.Z}) * TwistAxis};

	return FQuat{Projection.X, Projection.Y, Projection.Z, Quaternion.W}.GetNormalized();
}
