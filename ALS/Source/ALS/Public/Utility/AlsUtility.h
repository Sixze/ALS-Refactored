#pragma once

#include "Engine/EngineTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AlsUtility.generated.h"

class ACharacter;

UCLASS()
class ALS_API UAlsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr float DebugDrawImpactPointSize{32.0f};
	static constexpr float DebugDrawThickness{1.0f};

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", Meta = (DefaultToSelf = "Character"))
	static float GetAnimationCurveValue(UPARAM(DisplayName = "Character [self]") const ACharacter* Character, FName CurveName);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", Meta = (DefaultToSelf = "Actor"))
	static bool ShouldDisplayDebug(UPARAM(DisplayName = "Actor [self]") const AActor* Actor, FName DisplayName);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext", DevelopmentOnly))
	static void DrawDebugSweptSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                 const FLinearColor& Color, float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext", DevelopmentOnly))
	static void DrawDebugSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                       bool bHit, const FHitResult& Hit, FLinearColor SweepColor, FLinearColor HitColor,
	                                       float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext", DevelopmentOnly))
	static void DrawDebugSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                        float HalfHeight, bool bHit, const FHitResult& Hit, FLinearColor SweepColor,
	                                        FLinearColor HitColor, float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);
};
