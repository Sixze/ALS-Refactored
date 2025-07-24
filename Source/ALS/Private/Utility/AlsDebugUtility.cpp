#include "Utility/AlsDebugUtility.h"

#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Utility/AlsMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsDebugUtility)

bool UAlsDebugUtility::ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName)
{
	const auto* World{IsValid(Actor) ? Actor->GetWorld() : nullptr};
	const auto* Player{IsValid(World) ? World->GetFirstPlayerController() : nullptr};
	auto* Hud{IsValid(Player) ? Player->GetHUD() : nullptr};

	return IsValid(Hud) && Hud->ShouldDisplayDebug(DisplayName) && Hud->GetCurrentDebugTargetActor() == Actor;
}

void UAlsDebugUtility::DrawHalfCircle(const UObject* WorldContext, const FVector& Location, const FVector& XAxis,
                                      const FVector& YAxis, const float Radius, const FLinearColor& Color,
                                      const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	auto PreviousVertex{Location + XAxis * Radius};

	for (auto i{1}; i <= DrawCircleSidesCount / 2; i++)
	{
		static constexpr auto DeltaRadian{UE_TWO_PI / DrawCircleSidesCount};

		float Sin, Cos;
		FMath::SinCos(&Sin, &Cos, DeltaRadian * static_cast<float>(i));

		const auto NextVertex{Location + Radius * Cos * XAxis + Radius * Sin * YAxis};

		DrawDebugLine(World, PreviousVertex, NextVertex, FColor, bPersistent, Duration, DepthPriority, Thickness);

		PreviousVertex = NextVertex;
	}
#endif
}

void UAlsDebugUtility::DrawQuarterCircle(const UObject* WorldContext, const FVector& Location, const FVector& XAxis,
                                         const FVector& YAxis, const float Radius, const FLinearColor& Color,
                                         const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	auto PreviousVertex{Location + XAxis * Radius};

	for (auto i{1}; i <= DrawCircleSidesCount / 4; i++)
	{
		static constexpr auto DeltaRadian{UE_TWO_PI / DrawCircleSidesCount};

		float Sin, Cos;
		FMath::SinCos(&Sin, &Cos, DeltaRadian * static_cast<float>(i));

		const auto NextVertex{Location + Radius * Cos * XAxis + Radius * Sin * YAxis};

		DrawDebugLine(World, PreviousVertex, NextVertex, FColor, bPersistent, Duration, DepthPriority, Thickness);

		PreviousVertex = NextVertex;
	}
#endif
}

void UAlsDebugUtility::DrawSphereAlternative(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation,
                                             const float Radius, const FLinearColor& Color, const float Duration,
                                             const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const FRotationMatrix RotationMatrix{Rotation};

	FVector XAxis, YAxis, ZAxis;
	RotationMatrix.GetScaledAxes(XAxis, YAxis, ZAxis);

	DrawCircle(World, Location, XAxis, YAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
	DrawCircle(World, Location, XAxis, ZAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
	DrawCircle(World, Location, YAxis, ZAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
#endif
}

void UAlsDebugUtility::DrawLineTraceSingle(const UObject* WorldContext, const FVector& Start, const FVector& End, const bool bHit,
                                           const FHitResult& Hit, const FLinearColor& TraceColor, const FLinearColor& HitColor,
                                           const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto bPersistent{Duration < 0.0f};

	DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, Duration, DepthPriority, Thickness);

	if (bHit && Hit.bBlockingHit)
	{
		DrawDebugPoint(World, Hit.ImpactPoint, DrawImpactPointSize, HitColor.ToFColor(true), bPersistent, Duration, DepthPriority);
	}
#endif
}

void UAlsDebugUtility::DrawSweepSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, const float Radius,
                                       const FLinearColor& Color, const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const auto SweepVector{End - Start};

	DrawDebugCapsule(World, Start + SweepVector * 0.5f, UE_REAL_TO_FLOAT(SweepVector.Size()) * 0.5f + Radius,
	                 Radius, FRotationMatrix::MakeFromZ(SweepVector).ToQuat(),
	                 FColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugDirectionalArrow(World, Start, End, DrawArrowSize, FColor, bPersistent, Duration, DepthPriority, Thickness);
#endif
}

void UAlsDebugUtility::DrawSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                             const float Radius, const bool bHit, const FHitResult& Hit,
                                             const FLinearColor& SweepColor, const FLinearColor& HitColor,
                                             const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	DrawSweepSphere(World, Start, End, Radius, SweepColor.ToFColor(true), Duration, Thickness, DepthPriority);

	if (bHit && Hit.bBlockingHit)
	{
		const auto HitFColor{HitColor.ToFColor(true)};

		DrawSphereAlternative(World, Hit.Location, (End - Start).ToOrientationRotator(),
		                      Radius, HitFColor, Duration, Thickness, DepthPriority);

		DrawDebugPoint(World, Hit.ImpactPoint, DrawImpactPointSize, HitFColor, Duration < 0.0f, Duration, DepthPriority);
	}
#endif
}

void UAlsDebugUtility::DrawSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                              const FRotator& Rotation, const float Radius, const float HalfHeight, const bool bHit,
                                              const FHitResult& Hit, const FLinearColor& SweepColor, const FLinearColor& HitColor,
                                              const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto SweepFColor{SweepColor.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const auto Quaternion{Rotation.Quaternion()};

	DrawDebugCapsule(World, Start, HalfHeight, Radius, Quaternion, SweepFColor, bPersistent, Duration, DepthPriority, Thickness);
	DrawDebugCapsule(World, End, HalfHeight, Radius, Quaternion, SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugDirectionalArrow(World, Start, End, DrawArrowSize, SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	if (bHit && Hit.bBlockingHit)
	{
		const auto HitFColor{HitColor.ToFColor(true)};

		DrawDebugCapsule(World, Hit.Location, HalfHeight, Radius, Quaternion,
		                 HitFColor, bPersistent, Duration, DepthPriority, Thickness);

		DrawDebugPoint(World, Hit.ImpactPoint, DrawImpactPointSize, HitFColor, bPersistent, Duration, DepthPriority);
	}
#endif
}

void UAlsDebugUtility::DrawSweepSingleCapsuleAlternative(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                                         const float Radius, const float HalfHeight, const bool bHit, const FHitResult& Hit,
                                                         const FLinearColor& SweepColor, const FLinearColor& HitColor,
                                                         const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{IsValid(WorldContext) ? WorldContext->GetWorld() : nullptr};
	if (!ALS_ENSURE(IsValid(World)))
	{
		return;
	}

	const auto SweepFColor{SweepColor.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const auto Rotation{(End - Start).ToOrientationQuat()};
	const FQuatRotationMatrix RotationMatrix{Rotation};

	FVector XAxis, YAxis, ZAxis;
	RotationMatrix.GetScaledAxes(XAxis, YAxis, ZAxis);

	const auto XAxisInverse{-XAxis};
	const auto ZAxisInverse{-ZAxis};

	const auto DistanceToHemisphere{FMath::Max(1.0f, HalfHeight - Radius)};

	const auto StartTop{Start + DistanceToHemisphere * ZAxis};
	const auto StartBottom{Start - DistanceToHemisphere * ZAxis};

	const auto EndTop{End + DistanceToHemisphere * ZAxis};
	const auto EndBottom{End - DistanceToHemisphere * ZAxis};

	DrawHalfCircle(World, StartTop, YAxis, XAxisInverse, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)
	DrawHalfCircle(World, StartTop, YAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)

	DrawQuarterCircle(World, StartTop, XAxisInverse, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawHalfCircle(World, StartBottom, YAxis, XAxisInverse, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)
	DrawHalfCircle(World, StartBottom, YAxis, ZAxisInverse, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)

	DrawQuarterCircle(World, StartBottom, XAxisInverse, ZAxisInverse, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawHalfCircle(World, EndTop, YAxis, XAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)
	DrawHalfCircle(World, EndTop, YAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)

	DrawQuarterCircle(World, EndTop, XAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawHalfCircle(World, EndBottom, YAxis, XAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)
	DrawHalfCircle(World, EndBottom, YAxis, ZAxisInverse, Radius, SweepFColor, Duration, Thickness, DepthPriority); // NOLINT(readability-suspicious-call-argument)

	DrawQuarterCircle(World, EndBottom, XAxis, ZAxisInverse, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawDebugLine(World, StartTop - Radius * XAxis, StartBottom - Radius * XAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartTop + Radius * YAxis, StartBottom + Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartTop - Radius * YAxis, StartBottom - Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, EndTop + Radius * XAxis, EndBottom + Radius * XAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, EndTop + Radius * YAxis, EndBottom + Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, EndTop - Radius * YAxis, EndBottom - Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartTop + Radius * ZAxis, EndTop + Radius * ZAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartTop - Radius * YAxis, EndTop - Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartTop + Radius * YAxis, EndTop + Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartBottom - Radius * ZAxis, EndBottom - Radius * ZAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartBottom + Radius * YAxis, EndBottom + Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, StartBottom - Radius * YAxis, EndBottom - Radius * YAxis,
	              SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugDirectionalArrow(World, Start, End, DrawArrowSize, SweepFColor, bPersistent, Duration, DepthPriority, Thickness);

	if (bHit && Hit.bBlockingHit)
	{
		const auto HitFColor{HitColor.ToFColor(true)};

		DrawDebugCapsule(World, Hit.Location, HalfHeight, Radius, Rotation,
		                 HitFColor, bPersistent, Duration, DepthPriority, Thickness);

		DrawDebugPoint(World, Hit.ImpactPoint, DrawImpactPointSize, HitFColor, bPersistent, Duration, DepthPriority);
	}
#endif
}
