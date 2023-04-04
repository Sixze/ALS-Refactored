#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsUtility.generated.h"

DECLARE_STATS_GROUP(TEXT("Als"), STATGROUP_Als, STATCAT_Advanced)

UCLASS()
class ALS_API UAlsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static constexpr auto DrawImpactPointSize{32.0f};
	inline static constexpr auto DrawLineThickness{1.0f};
	inline static constexpr auto DrawArrowSize{50.0f};
	inline static constexpr auto DrawCircleSidesCount{16};

public:
	static constexpr FStringView BoolToString(bool bValue);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", Meta = (AutoCreateRefTerm = "Name", ReturnDisplayName = "Display String"))
	static FString NameToDisplayString(const FName& Name, bool bNameIsBool);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility",
		Meta = (DefaultToSelf = "Character", AutoCreateRefTerm = "CurveName", ReturnDisplayName = "Curve Value"))
	static float GetAnimationCurveValueFromCharacter(const ACharacter* Character, const FName& CurveName);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", Meta = (AutoCreateRefTerm = "Tag", ReturnDisplayName = "Child Tags"))
	static FGameplayTagContainer GetChildTags(const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", Meta = (AutoCreateRefTerm = "Tag", ReturnDisplayName = "Tag Name"))
	static FName GetSimpleTagName(const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext", ReturnDisplayName = "Ping"))
	static float GetFirstPlayerPingSeconds(const UObject* WorldContext);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility",
		Meta = (DefaultToSelf = "Actor", AutoCreateRefTerm = "DisplayName", ReturnDisplayName = "Value"))
	static bool ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Location, XAxis, YAxis, Color"))
	static void DrawHalfCircle(const UObject* WorldContext, const FVector& Location, const FVector& XAxis,
	                           const FVector& YAxis, float Radius, const FLinearColor& Color,
	                           float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Location, XAxis, YAxis, Color"))
	static void DrawQuarterCircle(const UObject* WorldContext, const FVector& Location, const FVector& XAxis,
	                              const FVector& YAxis, float Radius, const FLinearColor& Color,
	                              float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Location, Rotation, Color"))
	static void DrawDebugSphereAlternative(const UObject* WorldContext, const FVector& Location, const FRotator& Rotation,
	                                       float Radius, const FLinearColor& Color, float Duration = 0.0f,
	                                       float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Start, End, Color"))
	static void DrawDebugSweptSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                 const FLinearColor& Color, float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Start, End, TraceColor, HitColor"))
	static void DrawDebugLineTraceSingle(const UObject* WorldContext, const FVector& Start, const FVector& End, bool bHit,
	                                     const FHitResult& Hit, const FLinearColor& TraceColor, const FLinearColor& HitColor,
	                                     float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Start, End, SweepColor, HitColor"))
	static void DrawDebugSweepSingleSphere(const UObject* WorldContext, const FVector& Start, const FVector& End, float Radius,
	                                       bool bHit, const FHitResult& Hit, const FLinearColor& SweepColor, const FLinearColor& HitColor,
	                                       float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Start, End, Rotation, SweepColor, HitColor"))
	static void DrawDebugSweepSingleCapsule(const UObject* WorldContext, const FVector& Start, const FVector& End,
	                                        const FRotator& Rotation, float Radius, float HalfHeight, bool bHit,
	                                        const FHitResult& Hit, const FLinearColor& SweepColor, const FLinearColor& HitColor,
	                                        float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Utility", Meta = (WorldContext = "WorldContext",
		DevelopmentOnly, AutoCreateRefTerm = "Start, End, SweepColor, HitColor"))
	static void DrawDebugSweepSingleCapsuleAlternative(const UObject* WorldContext, const FVector& Start, const FVector& End,
	                                                   float Radius, float HalfHeight, bool bHit, const FHitResult& Hit,
	                                                   const FLinearColor& SweepColor, const FLinearColor& HitColor,
	                                                   float Duration = 0.0f, float Thickness = 1.0f, uint8 DepthPriority = 0);
};

constexpr FStringView UAlsUtility::BoolToString(const bool bValue)
{
	return bValue ? TEXTVIEW("True") : TEXTVIEW("False");
}
