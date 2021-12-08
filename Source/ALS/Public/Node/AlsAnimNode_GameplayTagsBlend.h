#pragma once

#include "GameplayTagContainer.h"
#include "AnimNodes/AnimNode_BlendListBase.h"

#include "AlsAnimNode_GameplayTagsBlend.generated.h"

USTRUCT()
struct ALS_API FAlsAnimNode_GameplayTagsBlend : public FAnimNode_BlendListBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<FGameplayTag> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AlwaysAsPin))
	FGameplayTag ActiveTag;

protected:
	virtual int32 GetActiveChildIndex() override;

public:
#if WITH_EDITOR
	void RefreshPoses();
#endif
};
