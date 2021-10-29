#pragma once

#include "AnimationModifier.h"

#include "AlsAnimationModifier_CopyCurves.generated.h"

UCLASS(DisplayName = "Als Copy Curves Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CopyCurves : public UAnimationModifier
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	UAnimSequence* SourceSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bCopyAllCurves{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess, EditCondition = "!bCopyAllCurves"))
	TArray<FName> CurveNames;

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
