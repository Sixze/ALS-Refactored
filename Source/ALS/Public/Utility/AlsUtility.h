#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsUtility.generated.h"

struct FBasedMovementInfo;

DECLARE_STATS_GROUP(TEXT("Als"), STATGROUP_Als, STATCAT_Advanced)

UCLASS()
class ALS_API UAlsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr FStringView BoolToString(bool bValue);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (AutoCreateRefTerm = "Name", ReturnDisplayName = "Display String"))
	static FString NameToDisplayString(const FName& Name, bool bNameIsBool);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility",
		Meta = (DefaultToSelf = "Character", AutoCreateRefTerm = "CurveName", ReturnDisplayName = "Curve Value"))
	static float GetAnimationCurveValueFromCharacter(const ACharacter* Character, const FName& CurveName);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (AutoCreateRefTerm = "Tag", ReturnDisplayName = "Child Tags"))
	static FGameplayTagContainer GetChildTags(const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (AutoCreateRefTerm = "Tag", ReturnDisplayName = "Tag Name"))
	static FName GetSimpleTagName(const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, Category = "ALS|Utility", Meta = (WorldContext = "WorldContext", ReturnDisplayName = "Ping"))
	static float GetFirstPlayerPingSeconds(const UObject* WorldContext);

	static bool TryGetMovementBaseRotationSpeed(const FBasedMovementInfo& BasedMovement, FRotator& RotationSpeed);
};

constexpr FStringView UAlsUtility::BoolToString(const bool bValue)
{
	return bValue ? TEXTVIEW("True") : TEXTVIEW("False");
}
