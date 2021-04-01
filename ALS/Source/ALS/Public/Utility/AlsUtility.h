#pragma once

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AlsUtility.generated.h"

UCLASS()
class ALS_API UAlsUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Als Utility", meta = (DefaultToSelf = "Character"))
	static float GetAnimationCurveValue(UPARAM(DisplayName = "Character [self]") const ACharacter* Character, FName CurveName);
};

inline float UAlsUtility::GetAnimationCurveValue(const ACharacter* Character, const FName CurveName)
{
	return ensure(IsValid(Character)) ? Character->GetMesh()->GetAnimInstance()->GetCurveValue(CurveName) : 0.0f;
}
