#include "Nodes/AlsAnimGraphNode_GameplayTagsBlend.h"

#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimGraphNode_GameplayTagsBlend)

#define LOCTEXT_NAMESPACE "AlsAnimGraphNode_GameplayTagsBlend"

UAlsAnimGraphNode_GameplayTagsBlend::UAlsAnimGraphNode_GameplayTagsBlend()
{
	Node.AddPose();
}

void UAlsAnimGraphNode_GameplayTagsBlend::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAlsAnimNode_GameplayTagsBlend, Tags))
	{
		ReconstructNode();
	}

	Super::PostEditChangeProperty(ChangedEvent);
}

FText UAlsAnimGraphNode_GameplayTagsBlend::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Blend Poses by Gameplay Tag");
}

FText UAlsAnimGraphNode_GameplayTagsBlend::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Blend Poses by Gameplay Tag");
}

void UAlsAnimGraphNode_GameplayTagsBlend::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins)
{
	Node.RefreshPosePins();

	Super::ReallocatePinsDuringReconstruction(PreviousPins);
}

FText UAlsAnimGraphNode_GameplayTagsBlend::GetMenuCategory() const
{
	return LOCTEXT("Category", "ALS");
}

FString UAlsAnimGraphNode_GameplayTagsBlend::GetNodeCategory() const
{
	return GetMenuCategory().ToString();
}

void UAlsAnimGraphNode_GameplayTagsBlend::CustomizePinData(UEdGraphPin* Pin, const FName SourcePropertyName, const int32 PinIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, PinIndex);

	bool bBlendPosePin;
	bool bBlendTimePin;
	GetBlendPinProperties(Pin, bBlendPosePin, bBlendTimePin);

	if (!bBlendPosePin && !bBlendTimePin)
	{
		return;
	}

	Pin->PinFriendlyName = PinIndex <= 0
		                       ? LOCTEXT("Default", "Default")
		                       : PinIndex > Node.Tags.Num()
		                       ? LOCTEXT("Invalid", "Invalid")
		                       : FText::AsCultureInvariant(UAlsUtility::GetSimpleTagName(Node.Tags[PinIndex - 1]).ToString());

	if (bBlendPosePin)
	{
		static const FTextFormat BlendPosePinFormat{LOCTEXT("Pose", "{PinName} Pose")};

		Pin->PinFriendlyName = FText::Format(BlendPosePinFormat, {{FString{TEXTVIEW("PinName")}, Pin->PinFriendlyName}});
	}
	else if (bBlendTimePin)
	{
		static const FTextFormat BlendTimePinFormat{LOCTEXT("BlendTime", "{PinName} Blend Time")};

		Pin->PinFriendlyName = FText::Format(BlendTimePinFormat, {{FString{TEXTVIEW("PinName")}, Pin->PinFriendlyName}});
	}
}

void UAlsAnimGraphNode_GameplayTagsBlend::GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin)
{
	const auto PinFullName{Pin->PinName.ToString()};
	const auto SeparatorIndex{PinFullName.Find(TEXTVIEW("_"), ESearchCase::CaseSensitive)};

	if (SeparatorIndex <= 0)
	{
		bBlendPosePin = false;
		bBlendTimePin = false;
		return;
	}

	const auto PinName{PinFullName.Left(SeparatorIndex)};
	bBlendPosePin = PinName == TEXTVIEW("BlendPose");
	bBlendTimePin = PinName == TEXTVIEW("BlendTime");
}

#undef LOCTEXT_NAMESPACE
