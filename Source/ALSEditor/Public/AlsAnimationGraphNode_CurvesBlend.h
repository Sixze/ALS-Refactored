#pragma once

#include "AnimGraphNode_BlendListBase.h"
#include "AnimationNode/AlsAnimationNode_CurvesBlend.h"

#include "AlsAnimationGraphNode_CurvesBlend.generated.h"

UCLASS()
class ALSEDITOR_API UAlsAnimationGraphNode_CurvesBlend : public UAnimGraphNode_BlendListBase
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
