#pragma once

#include "AnimationModifier.h"
#include "AlsAnimationModifier_CopyCurves.generated.h"

UCLASS(DisplayName = "Als Copy Curves Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CopyCurves : public UAnimationModifier
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSoftObjectPtr<UAnimSequence> SourceSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bCopyAllCurves{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (EditCondition = "!bCopyAllCurves"))
	TArray<FName> CurveNames;

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;

private:
	static void CopyCurve(UAnimSequence* SourceSequence, UAnimSequence* TargetSequence, const FName& CurveName);
};
