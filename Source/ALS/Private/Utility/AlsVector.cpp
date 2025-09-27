#include "Utility/AlsVector.h"

#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsVector)

FVector UAlsVector::SlerpSkipNormalization(const FVector& From, const FVector& To, const float Ratio)
{
	// http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/

	auto Dot{From | To};

	if (Dot > 0.9995f)
	{
		return FMath::Lerp(From, To, Ratio).GetSafeNormal();
	}

	Dot = FMath::Max(-1.0f, Dot);

	const auto Theta{UE_REAL_TO_FLOAT(FMath::Acos(Dot)) * Ratio};

	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Theta);

	auto FromPerpendicular{To - From * Dot};
	FromPerpendicular.Normalize();

	return From * Cos + FromPerpendicular * Sin;
}

FVector UAlsVector::SpringDamperVector(FAlsSpringVectorState& SpringState, const FVector& Current,
                                       const FVector& Target, const float DeltaTime, const float Frequency,
                                       const float DampingRatio, const float TargetVelocityAmount)
{
	return UAlsMath::SpringDamper(SpringState, Current, Target, DeltaTime, Frequency, DampingRatio, TargetVelocityAmount);
}
