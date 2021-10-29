#pragma once

#include "AnimGraphNode_BlendListBase.h"
#include "Node/AlsAnimNode_CurvesBlend.h"

#include "AlsAnimGraphNode_CurvesBlend.generated.h"

UCLASS()
class ALSEDITOR_API UAlsAnimGraphNode_CurvesBlend : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsAnimNode_CurvesBlend Node;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual FString GetNodeCategory() const override;

#if ENGINE_MAJOR_VERSION >= 5
	virtual void GetOutputLinkAttributes(FNodeAttributeArray& Attributes) const override;
#endif
};
