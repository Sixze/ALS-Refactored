#include "Nodes/AlsAnimGraphNode_SuppressAdditiveWarnings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimGraphNode_SuppressAdditiveWarnings)

#define LOCTEXT_NAMESPACE "AlsSuppressAdditiveWarningsAnimationGraphNode"

FText UAlsAnimGraphNode_SuppressAdditiveWarnings::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Suppress Additive Warnings");
}

FLinearColor UAlsAnimGraphNode_SuppressAdditiveWarnings::GetNodeTitleColor() const
{
	return FLinearColor::Red;
}

FText UAlsAnimGraphNode_SuppressAdditiveWarnings::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Suppress Additive Warnings");
}

FString UAlsAnimGraphNode_SuppressAdditiveWarnings::GetNodeCategory() const
{
	return FString{TEXTVIEW("ALS")};
}

void UAlsAnimGraphNode_SuppressAdditiveWarnings::PostProcessPinName(const UEdGraphPin* Pin, FString& DisplayName) const
{
	DisplayName = TEXTVIEW("");
}

#undef LOCTEXT_NAMESPACE
