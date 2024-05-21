#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsDebugUtility.generated.h"

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
	UFUNCTION(BlueprintPure, Category = "ALS|Debug Utility",
		Meta = (DefaultToSelf = "Actor", AutoCreateRefTerm = "DisplayName", ReturnDisplayName = "Value"))
	static bool ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, XAxis, YAxis, Color"))
	static void DrawHalfCircle(const UObject* WorldContext, const FVector& Location, const FVector& XAxis,
	                           const FVector& YAxis, float Radius, const FLinearColor& Color,
	                           float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, XAxis, YAxis, Color"))
	static void DrawQuarterCircle(const UObject* WorldContext, const FVector& Location, const FVector& XAxis,
	                              const FVector& YAxis, float Radius, const FLinearColor& Color,
	                              float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Location, Rotation, Color"))
	static void DrawSphereAlternative(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation,
	                                  float Radius, const FLinearColor& Color, float Duration = 0.0f,
	                                  float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, Color"))
	static void DrawSweepSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                            const FLinearColor& Color, float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, TraceColor, HitColor"))
	static void DrawLineTraceSingle(const UObject* WorldContext, const FVector& Start, const FVector& End, bool bHit,
	                                const FHitResult& Hit, const FLinearColor& TraceColor, const FLinearColor& HitColor,
	                                float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, SweepColor, HitColor"))
	static void DrawSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                  bool bHit, const FHitResult& Hit, const FLinearColor& SweepColor, const FLinearColor& HitColor,
	                                  float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, Rotation, SweepColor, HitColor"))
	static void DrawSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End,
	                                   const FRotator& Rotation, float Radius, float HalfHeight, bool bHit,
	                                   const FHitResult& Hit, const FLinearColor& SweepColor, const FLinearColor& HitColor,
	                                   float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Debug Utility",
		Meta = (WorldContext = "WorldContext", DevelopmentOnly, AutoCreateRefTerm = "Start, End, SweepColor, HitColor"))
	static void DrawSweepSingleCapsuleAlternative(const UObject* WorldContext, const FVector& Start, const FVector& End,
	                                              float Radius, float HalfHeight, bool bHit, const FHitResult& Hit,
	                                              const FLinearColor& SweepColor, const FLinearColor& HitColor,
	                                              float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);
};
