#include "AlsAnimationGraphNode_CurvesBlend.h"

#define LOCTEXT_NAMESPACE "BlendCurvesAnimationGraphNode"

FText UAlsAnimationGraphNode_CurvesBlend::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Blend Curves");
}

FText UAlsAnimationGraphNode_CurvesBlend::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Blend Curves");
}

FString UAlsAnimationGraphNode_CurvesBlend::GetNodeCategory() const
{
	return TEXT("ALS");
}

#undef LOCTEXT_NAMESPACE
