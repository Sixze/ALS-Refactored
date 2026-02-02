#include "Nodes/AlsAnimGraphNode_CurvesBlend.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimGraphNode_CurvesBlend)

#define LOCTEXT_NAMESPACE "AlsAnimGraphNode_CurvesBlend"

FText UAlsAnimGraphNode_CurvesBlend::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Blend Curves");
}

FLinearColor UAlsAnimGraphNode_CurvesBlend::GetNodeTitleColor() const
{
	return {0.1f, 0.4f, 0.8f};
}

FText UAlsAnimGraphNode_CurvesBlend::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Blend Curves");
}

FText UAlsAnimGraphNode_CurvesBlend::GetMenuCategory() const
{
	return LOCTEXT("Category", "ALS");
}

FString UAlsAnimGraphNode_CurvesBlend::GetNodeCategory() const
{
	return GetMenuCategory().ToString();
}

#undef LOCTEXT_NAMESPACE
