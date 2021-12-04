#include "Utility/AlsUtility.h"

#include "DrawDebugHelpers.h"
#include "GameplayTagsManager.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/AlsMath.h"

FString UAlsUtility::NameToDisplayString(const FName& Name, const bool bIsBool)
{
	return FName::NameToDisplayString(Name.ToString(), bIsBool);
}

float UAlsUtility::GetAnimationCurveValue(const ACharacter* Character, const FName& CurveName)
{
	return ensure(IsValid(Character)) ? Character->GetMesh()->GetAnimInstance()->GetCurveValue(CurveName) : 0.0f;
}

FGameplayTagContainer UAlsUtility::GetChildTags(const FGameplayTag& Tag)
{
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(Tag);
}

FName UAlsUtility::GetSimpleTagName(const FGameplayTag& Tag)
{
	const auto TagNode{UGameplayTagsManager::Get().FindTagNode(Tag)};

	return TagNode.IsValid() ? TagNode->GetSimpleTagName() : NAME_None;
}

float UAlsUtility::GetFirstPlayerPingSeconds(const UObject* WorldContext)
{
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return 0.0f;
	}

	const auto* PlayerController{World->GetFirstPlayerController()};
	if (!IsValid(PlayerController) || !IsValid(PlayerController->PlayerState))
	{
		return 0.0f;
	}

	return PlayerController->PlayerState->ExactPing * 0.001f;
}

bool UAlsUtility::ShouldDisplayDebug(const AActor* Actor, const FName& DisplayName)
{
	if (!IsValid(Actor))
	{
		return false;
	}

	const auto* PlayerController{Actor->GetWorld()->GetFirstPlayerController()};
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetHUD()))
	{
		return false;
	}

	return PlayerController->GetHUD()->ShouldDisplayDebug(DisplayName) &&
	       PlayerController->GetHUD()->GetCurrentDebugTargetActor() == Actor;
}

void UAlsUtility::DrawHalfCircle(const UObject* WorldContext, const FVector& Center, const FVector& XAxis,
                                 const FVector& YAxis, const float Radius, const FLinearColor& Color,
                                 const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	constexpr auto AngleDelta{UAlsMath::TwoPi / DrawCircleSidesCount};
	auto PreviousVertex{Center + XAxis * Radius};

	for (auto i{1}; i <= DrawCircleSidesCount / 2; i++)
	{
		float Sin, Cos;
		FMath::SinCos(&Sin, &Cos, AngleDelta * i);

		const auto NextVertex{Center + Radius * Cos * XAxis + Radius * Sin * YAxis};

		DrawDebugLine(World, PreviousVertex, NextVertex, FColor, bPersistent, Duration, DepthPriority, Thickness);

		PreviousVertex = NextVertex;
	}
#endif
}

void UAlsUtility::DrawQuarterCircle(const UObject* WorldContext, const FVector& Center, const FVector& XAxis,
                                    const FVector& YAxis, const float Radius, const FLinearColor& Color,
                                    const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	constexpr auto AngleDelta{UAlsMath::TwoPi / DrawCircleSidesCount};
	auto PreviousVertex{Center + XAxis * Radius};

	for (auto i{1}; i <= DrawCircleSidesCount / 4; i++)
	{
		float Sin, Cos;
		FMath::SinCos(&Sin, &Cos, AngleDelta * i);

		const auto NextVertex{Center + Radius * Cos * XAxis + Radius * Sin * YAxis};

		DrawDebugLine(World, PreviousVertex, NextVertex, FColor, bPersistent, Duration, DepthPriority, Thickness);

		PreviousVertex = NextVertex;
	}
#endif
}

void UAlsUtility::DrawDebugSphereAlternative(const UObject* WorldContext, const FVector& Center, const FRotator& Rotation,
                                             const float Radius, const FLinearColor& Color, const float Duration,
                                             const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const FRotationMatrix RotationMatrix{Rotation};

	FVector XAxis, YAxis, ZAxis;
	RotationMatrix.GetScaledAxes(XAxis, YAxis, ZAxis);

	DrawCircle(World, Center, XAxis, YAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
	DrawCircle(World, Center, XAxis, ZAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
	DrawCircle(World, Center, YAxis, ZAxis, FColor, Radius, DrawCircleSidesCount, bPersistent, Duration, DepthPriority, Thickness);
#endif
}

void UAlsUtility::DrawDebugLineTraceSingle(const UObject* WorldContext, const FVector& Start, const FVector& End, const bool bHit,
                                           const FHitResult& Hit, const FLinearColor& TraceColor, const FLinearColor& HitColor,
                                           const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto bPersistent{Duration < 0.0f};

	DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, Duration, DepthPriority, Thickness);

	if (bHit && Hit.bBlockingHit)
	{
		DrawDebugPoint(World, Hit.ImpactPoint, DrawImpactPointSize,
		               HitColor.ToFColor(true), bPersistent, Duration, DepthPriority);
	}
#endif
}

void UAlsUtility::DrawDebugSweptSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, const float Radius,
                                       const FLinearColor& Color, const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto FColor{Color.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const auto AxisVector{End - Start};

	DrawDebugCapsule(World, Start + AxisVector * 0.5f, AxisVector.Size() * 0.5f + Radius,
	                 Radius, FRotationMatrix::MakeFromZ(AxisVector).ToQuat(),
	                 FColor, bPersistent, Duration, DepthPriority, Thickness);

	DrawDebugDirectionalArrow(World, Start, End, DrawArrowSize, FColor, bPersistent, Duration, DepthPriority, Thickness);
#endif
}

void UAlsUtility::DrawDebugSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                             const float Radius, const bool bHit, const FHitResult& Hit,
                                             const FLinearColor& SweepColor, const FLinearColor& HitColor,
                                             const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	DrawDebugSweptSphere(World, Start, End, Radius, SweepColor.ToFColor(true), Duration, Thickness, DepthPriority);

	if (bHit && Hit.bBlockingHit)
	{
		const auto HitFColor{HitColor.ToFColor(true)};

		DrawDebugSphereAlternative(World, Hit.Location, (End - Start).ToOrientationRotator(),
		                           Radius, HitFColor, Duration, Thickness, DepthPriority);

		DrawDebugPoint(World, Hit.ImpactPoint, DrawImpactPointSize, HitFColor, Duration < 0.0f, Duration, DepthPriority);
	}
#endif
}

void UAlsUtility::DrawDebugSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                              const FRotator& Rotation, const float Radius, const float HalfHeight, const bool bHit,
                                              const FHitResult& Hit, const FLinearColor& SweepColor, const FLinearColor& HitColor,
                                              const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
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

void UAlsUtility::DrawDebugSweepSingleCapsuleAlternative(const UObject* WorldContext, const FVector& Start, const FVector& End,
                                                         const float Radius, const float HalfHeight, const bool bHit, const FHitResult& Hit,
                                                         const FLinearColor& SweepColor, const FLinearColor& HitColor,
                                                         const float Duration, const float Thickness, const uint8 DepthPriority)
{
#if ENABLE_DRAW_DEBUG
	const auto* World{WorldContext->GetWorld()};
	if (!IsValid(World))
	{
		return;
	}

	const auto SweepFColor{SweepColor.ToFColor(true)};
	const auto bPersistent{Duration < 0.0f};

	const auto Rotation{(End - Start).ToOrientationQuat()};
	const FQuatRotationMatrix RotationMatrix{Rotation};

	FVector XAxis, YAxis, ZAxis;
	RotationMatrix.GetScaledAxes(XAxis, YAxis, ZAxis);

	const auto NegatedXAxis{-XAxis};
	const auto NegatedZAxis{-ZAxis};

	const auto DistanceToHemisphere{FMath::Max(1.0f, HalfHeight - Radius)};

	const auto StartTop{Start + DistanceToHemisphere * ZAxis};
	const auto StartBottom{Start - DistanceToHemisphere * ZAxis};

	const auto EndTop{End + DistanceToHemisphere * ZAxis};
	const auto EndBottom{End - DistanceToHemisphere * ZAxis};

	DrawHalfCircle(World, StartTop, YAxis, NegatedXAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);
	DrawHalfCircle(World, StartTop, YAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawQuarterCircle(World, StartTop, NegatedXAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawHalfCircle(World, StartBottom, YAxis, NegatedXAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);
	DrawHalfCircle(World, StartBottom, YAxis, NegatedZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawQuarterCircle(World, StartBottom, NegatedXAxis, NegatedZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawHalfCircle(World, EndTop, YAxis, XAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);
	DrawHalfCircle(World, EndTop, YAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawQuarterCircle(World, EndTop, XAxis, ZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawHalfCircle(World, EndBottom, YAxis, XAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);
	DrawHalfCircle(World, EndBottom, YAxis, NegatedZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

	DrawQuarterCircle(World, EndBottom, XAxis, NegatedZAxis, Radius, SweepFColor, Duration, Thickness, DepthPriority);

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
