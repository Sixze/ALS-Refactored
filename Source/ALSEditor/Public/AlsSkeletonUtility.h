#pragma once

#include "Animation/Skeleton.h"
#include "AlsSkeletonUtility.generated.h"

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsBlendProfileEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	float BlendScale{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bIncludeDescendants{false};
};

UCLASS()
class ALSEDITOR_API UAlsSkeletonUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Skeleton Utility")
	static void AddAnimationCurves(USkeleton* Skeleton, const TArray<FName>& CurveNames);

	UFUNCTION(BlueprintCallable, Category = "ALS|Skeleton Utility")
	static void AddOrReplaceSlot(USkeleton* Skeleton, FName SlotName, FName GroupName);

	UFUNCTION(BlueprintCallable, Category = "ALS|Skeleton Utility", Meta = (AutoCreateRefTerm = "SourceBoneName, TargetBoneName"))
	static void AddOrReplaceVirtualBone(USkeleton* Skeleton, const FName& SourceBoneName,
	                                    const FName& TargetBoneName, FName VirtualBoneName);

	UFUNCTION(BlueprintCallable, Category = "ALS|Skeleton Utility",
		Meta = (AutoCreateRefTerm = "BoneName, RelativeLocation, RelativeRotation"))
	static void AddOrReplaceSocket(USkeleton* Skeleton, FName SocketName, const FName& BoneName,
	                               const FVector& RelativeLocation, const FRotator& RelativeRotation);

	UFUNCTION(BlueprintCallable, Category = "ALS|Skeleton Utility")
	static void AddOrReplaceWeightBlendProfile(USkeleton* Skeleton, FName BlendProfileName, const TArray<FAlsBlendProfileEntry>& Entries);

	UFUNCTION(BlueprintCallable, Category = "ALS|Skeleton Utility", Meta = (AutoCreateRefTerm = "BoneName"))
	static void SetBoneRetargetingMode(USkeleton* Skeleton, const FName& BoneName,
	                                   EBoneTranslationRetargetingMode::Type RetargetingMode, bool bIncludeDescendants);
};
