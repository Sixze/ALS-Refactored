#include "AlsAnimGraphNode_CurvesBlend.h"

#define LOCTEXT_NAMESPACE "CurvesBlendAnimationGraphNode"

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
	return TEXT("ALS");
}

#if ENGINE_MAJOR_VERSION >= 5
void UAlsAnimGraphNode_CurvesBlend::GetOutputLinkAttributes(FNodeAttributeArray& Attributes) const {}
#endif

#undef LOCTEXT_NAMESPACE
