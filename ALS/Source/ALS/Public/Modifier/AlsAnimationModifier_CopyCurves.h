#pragma once

#include "AnimationModifier.h"

#include "AlsAnimationModifier_CopyCurves.generated.h"

UCLASS(meta = (DisplayName = "Als Copy Curves Animation Modifier"))
class ALS_API UAlsAnimationModifier_CopyCurves : public UAnimationModifier
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	UAnimSequence* SourceSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess))
	bool bCopyAllCurves{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess, EditCondition = "!bCopyAllCurves"))
	TArray<FName> CurveNames;

protected:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
