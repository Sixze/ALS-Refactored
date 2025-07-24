#pragma once

#include "GameplayTagContainer.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "AlsAnimNode_GameplayTagsBlend.generated.h"

class UAlsAnimGraphNode_GameplayTagsBlend;

USTRUCT()
struct ALS_API FAlsAnimNode_GameplayTagsBlend : public FAnimNode_BlendListBase
{
	GENERATED_BODY()

	friend UAlsAnimGraphNode_GameplayTagsBlend;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty, PinShownByDefault))
	FGameplayTag ActiveTag;

	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty))
	TArray<FGameplayTag> Tags;
#endif

protected:
	virtual int32 GetActiveChildIndex() override;

public:
	const FGameplayTag& GetActiveTag() const;

	const TArray<FGameplayTag>& GetTags() const;

#if WITH_EDITOR
	void RefreshPosePins();
#endif
};
