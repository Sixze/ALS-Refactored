#include "Nodes/AlsAnimGraphNode_GameplayTagsBlend.h"

#include "Animation/AnimNode_Inertialization.h"
#include "Utility/AlsUtility.h"

#define LOCTEXT_NAMESPACE "GameplayTagsBlendAnimationGraphNode"

UAlsAnimGraphNode_GameplayTagsBlend::UAlsAnimGraphNode_GameplayTagsBlend()
{
	Node.AddPose();
}

void UAlsAnimGraphNode_GameplayTagsBlend::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FAlsAnimNode_GameplayTagsBlend, Tags))
	{
		ReconstructNode();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
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
	Node.RefreshPoses();

	Super::ReallocatePinsDuringReconstruction(PreviousPins);
}

FString UAlsAnimGraphNode_GameplayTagsBlend::GetNodeCategory() const
{
	return TEXT("ALS");
}

void UAlsAnimGraphNode_GameplayTagsBlend::CustomizePinData(UEdGraphPin* Pin, const FName SourcePropertyName, const int32 ArrayIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);

	bool bBlendPosePin;
	bool bBlendTimePin;
	GetBlendPinProperties(Pin, bBlendPosePin, bBlendTimePin);

	if (!bBlendPosePin && !bBlendTimePin)
	{
		return;
	}

	Pin->PinFriendlyName = ArrayIndex <= 0
		                       ? LOCTEXT("Default", "Default")
		                       : ArrayIndex > Node.Tags.Num()
		                       ? LOCTEXT("Invalid", "Invalid")
		                       : FText::AsCultureInvariant(UAlsUtility::GetSimpleTagName(Node.Tags[ArrayIndex - 1]).ToString());

	if (bBlendPosePin)
	{
		Pin->PinFriendlyName = FText::Format(LOCTEXT("Pose", "{PinName} Pose"), {{TEXT("PinName"), Pin->PinFriendlyName}});
	}
	else if (bBlendTimePin)
	{
		Pin->PinFriendlyName = FText::Format(LOCTEXT("BlendTime", "{PinName} Blend Time"), {{TEXT("PinName"), Pin->PinFriendlyName}});
	}
}

void UAlsAnimGraphNode_GameplayTagsBlend::GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin)
{
	const auto PinFullName{Pin->PinName.ToString()};
	const auto SeparatorIndex{PinFullName.Find(TEXT("_"), ESearchCase::CaseSensitive)};

	if (SeparatorIndex <= 0)
	{
		bBlendPosePin = false;
		bBlendTimePin = false;
		return;
	}

	const auto PinName{PinFullName.Left(SeparatorIndex)};
	bBlendPosePin = PinName == TEXT("BlendPose");
	bBlendTimePin = PinName == TEXT("BlendTime");
}

#undef LOCTEXT_NAMESPACE
