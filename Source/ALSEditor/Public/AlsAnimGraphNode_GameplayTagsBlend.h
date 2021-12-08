#pragma once

#include "AnimGraphNode_BlendListBase.h"
#include "Node/AlsAnimNode_GameplayTagsBlend.h"

#include "AlsAnimGraphNode_GameplayTagsBlend.generated.h"

UCLASS()
class ALSEDITOR_API UAlsAnimGraphNode_GameplayTagsBlend : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsAnimNode_GameplayTagsBlend Node;

public:
	UAlsAnimGraphNode_GameplayTagsBlend();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;

	virtual FString GetNodeCategory() const override;

	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;

#if ENGINE_MAJOR_VERSION >= 5
	virtual void GetOutputLinkAttributes(FNodeAttributeArray& Attributes) const override;
#endif

protected:
	static void GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin);
};
