#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsDebugUtility.generated.h"

struct FHitResult;

UCLASS()
class ALS_API UAlsDebugUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr auto DrawImpactPointSize{32.0f};
	static constexpr auto DrawLineThickness{1.0f};
	static constexpr auto DrawArrowSize{50.0f};
	static constexpr auto DrawCircleSidesCount{16};

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Debug Utility", Meta = (DefaultToSelf = "Actor", ReturnDisplayName = "Value"))
	static bool ShouldDisplayDebugForActor(const AActor* Actor, FName DisplayName);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, ForwardAxis, RightAxis"))
	static void DrawHalfCircle(const UObject* WorldContext, const FVector& Location, const FVector& ForwardAxis,
	                           const FVector& RightAxis, float Radius, FLinearColor Color,
	                           float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, ForwardAxis, RightAxis"))
	static void DrawQuarterCircle(const UObject* WorldContext, const FVector& Location, const FVector& ForwardAxis,
	                              const FVector& RightAxis, float Radius, FLinearColor Color,
	                              float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, Rotation"))
	static void DrawSphereAlternative(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation,
	                                  float Radius, FLinearColor Color, float Duration = 0.0f,
	                                  float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End"))
	static void DrawSweepSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                            FLinearColor Color, float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End"))
	static void DrawLineTraceSingle(const UObject* WorldContext, const FVector& Start, const FVector& End, bool bHit,
	                                const FHitResult& Hit, FLinearColor TraceColor, FLinearColor HitColor,
	                                float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End"))
	static void DrawSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                  bool bHit, const FHitResult& Hit, FLinearColor SweepColor, FLinearColor HitColor,
	                                  float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, Rotation"))
	static void DrawSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End,
	                                   const FRotator& Rotation, float Radius, float HalfHeight, bool bHit,
	                                   const FHitResult& Hit, FLinearColor SweepColor, FLinearColor HitColor,
	                                   float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End"))
	static void DrawSweepSingleCapsuleAlternative(const UObject* WorldContext, const FVector& Start, const FVector& End,
	                                              float Radius, float HalfHeight, bool bHit, const FHitResult& Hit,
	                                              FLinearColor SweepColor, FLinearColor HitColor, float Duration = 0.0f,
	                                              float Thickness = 1.0f, uint8 DepthPriority = 0);
};
