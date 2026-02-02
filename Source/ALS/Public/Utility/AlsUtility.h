#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsUtility.generated.h"

struct FBasedMovementInfo;
class ACharacter;

DECLARE_STATS_GROUP(TEXT("Als"), STATGROUP_Als, STATCAT_Advanced)

UCLASS()
class ALS_API UAlsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr FStringView BoolToString(bool bValue);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (ReturnDisplayName = "Display String"))
	static FString NameToDisplayString(FName Name, bool bNameIsBool);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (DefaultToSelf = "Character", ReturnDisplayName = "Curve Value"))
	static float GetAnimationCurveValueFromCharacter(const ACharacter* Character, FName CurveName);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (ReturnDisplayName = "Child Tags"))
	static FGameplayTagContainer GetChildTags(FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (ReturnDisplayName = "Tag Name"))
	static FName GetSimpleTagName(FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (WorldContext = "WorldContext", ReturnDisplayName = "Ping"))
	static float GetFirstPlayerPingSeconds(const UObject* WorldContext);

	static bool TryGetMovementBaseAngularVelocity(const FBasedMovementInfo& BasedMovement, FVector& AngularVelocity);
};

constexpr FStringView UAlsUtility::BoolToString(const bool bValue)
{
	return bValue ? TEXTVIEW("True") : TEXTVIEW("False");
}
