#include "Utility/AlsUtility.h"

#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"

float UAlsUtility::GetAnimationCurveValue(const ACharacter* Character, const FName& CurveName)
{
	return ensure(IsValid(Character)) ? Character->GetMesh()->GetAnimInstance()->GetCurveValue(CurveName) : 0.0f;
}

bool UAlsUtility::ShouldDisplayDebug(const AActor* Actor, const FName& DisplayName)
{
	const auto* FirstPlayerController{IsValid(Actor) ? UGameplayStatics::GetPlayerController(Actor->GetWorld(), 0) : nullptr};
	auto* Hud{IsValid(FirstPlayerController) ? FirstPlayerController->GetHUD() : nullptr};

	return IsValid(Hud) && Hud->ShouldDisplayDebug(DisplayName) && Hud->GetCurrentDebugTargetActor() == Actor;
}

void UAlsUtility::DrawDebugSweptSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, const float Radius,
                                       const FLinearColor& Color, const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto AxisVector{End - Start};

	DrawDebugCapsule(World, Start + AxisVector * 0.5f, AxisVector.Size() * 0.5f + Radius,
	                 Radius, FRotationMatrix::MakeFromZ(AxisVector).ToQuat(), FColor,
	                 Duration < 0.0f, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, Start, End, FColor, Duration < 0.0f, Duration, DepthPriority, Thickness);
#endif
}

void UAlsUtility::DrawDebugSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                             const float Radius, const bool bHit, const FHitResult& Hit,
                                             const FLinearColor SweepColor, const FLinearColor HitColor,
                                             const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	DrawDebugSweptSphere(World, Start, End, Radius, SweepColor.ToFColor(true), Duration, Thickness, DepthPriority);

	if (bHit && Hit.bBlockingHit)
	{
		const auto HitFColor{HitColor.ToFColor(true)};

		DrawDebugCapsule(World, Hit.Location, Radius, Radius, FQuat::Identity,
		                 HitFColor, Duration < 0.0f, Duration, DepthPriority, Thickness);

		DrawDebugPoint(World, Hit.ImpactPoint, DebugDrawImpactPointSize,
		               HitFColor, Duration < 0.0f, Duration, DepthPriority);
	}
#endif
}

void UAlsUtility::DrawDebugSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                              const float Radius, const float HalfHeight, const bool bHit,
                                              const FHitResult& Hit, const FLinearColor SweepColor, const FLinearColor HitColor,
                                              const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto SweepFColor{SweepColor.ToFColor(true)};

	DrawDebugCapsule(World, Start, HalfHeight, Radius, FQuat::Identity, SweepFColor,
	                 Duration < 0.0f, Duration, DepthPriority, Thickness);

	DrawDebugCapsule(World, End, HalfHeight, Radius, FQuat::Identity, SweepFColor,
	                 Duration < 0.0f, Duration, DepthPriority, Thickness);

	DrawDebugLine(World, Start, End, SweepFColor, Duration < 0.0f, Duration, DepthPriority, Thickness);

	if (bHit && Hit.bBlockingHit)
	{
		const auto HitFColor{HitColor.ToFColor(true)};

		DrawDebugCapsule(World, Hit.Location, HalfHeight, Radius, FQuat::Identity,
		                 HitFColor, Duration < 0.0f, Duration, DepthPriority, Thickness);

		DrawDebugPoint(World, Hit.ImpactPoint, DebugDrawImpactPointSize,
		               HitFColor, Duration < 0.0f, Duration, DepthPriority);
	}
#endif
}
