#include "Nodes/AlsAnimGraphNode_CurvesBlend.h"

#define LOCTEXT_NAMESPACE "AlsCurvesBlendAnimationGraphNode"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimGraphNode_CurvesBlend)

FText UAlsAnimGraphNode_CurvesBlend::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Blend Curves");
}

FText UAlsAnimGraphNode_CurvesBlend::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Blend Curves");
}

FString UAlsAnimGraphNode_CurvesBlend::GetNodeCategory() const
{
	return FString{TEXTVIEW("ALS")};
}

#undef LOCTEXT_NAMESPACE
