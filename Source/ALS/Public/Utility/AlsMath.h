#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "State/AlsMovementDirection.h"
#include "AlsMath.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsSpringFloatState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	float Velocity{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	float PreviousTarget{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bStateValid : 1 {false};

public:
	void Reset();
};

UCLASS(Meta = (BlueprintThreadSafe))
class ALS_API UAlsMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr auto Ln2{0.6931471805599453f}; // FMath::Loge(2.0f).

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Math Utility", Meta = (ReturnDisplayName = "Value"))
	static float Clamp01(float Value);

	UFUNCTION(BlueprintPure, Category = "ALS|Math Utility", Meta = (ReturnDisplayName = "Value"))
	static float LerpClamped(float From, float To, float Ratio);

	UFUNCTION(BlueprintPure, Category = "ALS|Math Utility", Meta = (ReturnDisplayName = "Alpha"))
	static float DamperExactAlpha(float DeltaTime, float HalfLife);

	// HalfLife is the time it takes for the distance to the target to be reduced by half.
	template <typename ValueType>
	static ValueType DamperExact(const ValueType& Current, const ValueType& Target, float DeltaTime, float HalfLife);

	template <typename ValueType, typename StateType>
	static ValueType SpringDamper(StateType& SpringState, const ValueType& Current, const ValueType& Target,
	                              float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

	UFUNCTION(BlueprintPure, Category = "ALS|Math Utility", Meta = (ReturnDisplayName = "Value"))
	static float SpringDamperFloat(UPARAM(ref) FAlsSpringFloatState& SpringState, float Current, float Target,
	                               float DeltaTime, float Frequency, float DampingRatio, float TargetVelocityAmount = 1.0f);

	UFUNCTION(BlueprintPure, Category = "ALS|Math Utility", Meta = (ReturnDisplayName = "Direction"))
	static EAlsMovementDirection CalculateMovementDirection(float Angle, float ForwardHalfAngle, float AngleThreshold);

	// Calculates the projection location and direction of the perpendicular to AC through B.
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utility",
		Meta = (AutoCreateRefTerm = "ALocation, BLocation, CLocation", ExpandBoolAsExecs = "ReturnValue"))
	static bool TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation, const FVector& CLocation,
	                                   FVector& ProjectionLocation, FVector& PoleDirection);
};

inline void FAlsSpringFloatState::Reset()
{
	Velocity = 0.f;
	PreviousTarget = 0.f;
	bStateValid = false;
}

inline float UAlsMath::Clamp01(const float Value)
{
	return Value > 0.0f
		       ? Value < 1.0f
			         ? Value
			         : 1.0f
		       : 0.0f;
}

inline float UAlsMath::LerpClamped(const float From, const float To, const float Ratio)
{
	return From + (To - From) * Clamp01(Ratio);
}

inline float UAlsMath::DamperExactAlpha(const float DeltaTime, const float HalfLife)
{
	// https://theorangeduck.com/page/spring-roll-call#exactdamper

	return 1.0f - FMath::InvExpApprox(Ln2 / (HalfLife + UE_SMALL_NUMBER) * DeltaTime);
}

template <typename ValueType>
ValueType UAlsMath::DamperExact(const ValueType& Current, const ValueType& Target, const float DeltaTime, const float HalfLife)
{
	return FMath::Lerp(Current, Target, DamperExactAlpha(DeltaTime, HalfLife));
}

template <typename ValueType, typename StateType>
ValueType UAlsMath::SpringDamper(StateType& SpringState, const ValueType& Current, const ValueType& Target, const float DeltaTime,
                                 const float Frequency, const float DampingRatio, const float TargetVelocityAmount)
{
	if (DeltaTime <= UE_SMALL_NUMBER)
	{
		return Current;
	}

	if (!SpringState.bStateValid)
	{
		SpringState.Velocity = ValueType{0.0f};
		SpringState.PreviousTarget = Target;
		SpringState.bStateValid = true;

		return Target;
	}

	ValueType Result{Current};
	FMath::SpringDamper(Result, SpringState.Velocity, Target,
	                    (Target - SpringState.PreviousTarget) * Clamp01(TargetVelocityAmount) / DeltaTime,
	                    DeltaTime, Frequency, DampingRatio);

	SpringState.PreviousTarget = Target;

	return Result;
}
