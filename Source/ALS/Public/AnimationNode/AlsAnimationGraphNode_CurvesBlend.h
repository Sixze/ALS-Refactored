#pragma once

#include "AlsAnimationNode_CurvesBlend.h"
#include "AnimGraphNode_BlendListBase.h"

#include "AlsAnimationGraphNode_CurvesBlend.generated.h"

UCLASS()
class ALS_API UAlsAnimationGraphNode_CurvesBlend : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsAnimationNode_CurvesBlend Node;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType1) const override;

	virtual FText GetTooltipText() const override;

	virtual FString GetNodeCategory() const override;
};
