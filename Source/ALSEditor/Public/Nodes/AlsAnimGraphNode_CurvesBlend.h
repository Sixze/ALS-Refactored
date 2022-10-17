#pragma once

#include "AnimGraphNode_Base.h"
#include "Nodes/AlsAnimNode_CurvesBlend.h"
#include "AlsAnimGraphNode_CurvesBlend.generated.h"

UCLASS()
class ALSEDITOR_API UAlsAnimGraphNode_CurvesBlend : public UAnimGraphNode_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsAnimNode_CurvesBlend Node;

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual FString GetNodeCategory() const override;
};
