#include "Utility/AlsVector.h"

#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsVector)

FVector UAlsVector::SlerpSkipNormalization(const FVector& From, const FVector& To, const float Ratio)
{
	// https://allenchou.net/2018/05/game-math-deriving-the-slerp-formula/

	const auto Dot{From | To};

	if (Dot > 0.9995f || Dot < -0.9995f)
	{
		return FMath::Lerp(From, To, Ratio).GetSafeNormal();
	}

	const auto Theta{UE_REAL_TO_FLOAT(FMath::Acos(Dot)) * Ratio};

	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Theta);

	const auto FromPerpendicular{(To - From * Dot).GetSafeNormal()};

	return From * Cos + FromPerpendicular * Sin;
}

FVector UAlsVector::SpringDamperVector(FAlsSpringVectorState& SpringState, const FVector& Current,
                                       const FVector& Target, const float DeltaTime, const float Frequency,
                                       const float DampingRatio, const float TargetVelocityAmount)
{
	return UAlsMath::SpringDamper(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}
