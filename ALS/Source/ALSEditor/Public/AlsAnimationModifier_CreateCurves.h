#pragma once

#include "AnimationModifier.h"
#include "Utility/AlsConstants.h"

#include "AlsAnimationModifier_CreateCurves.generated.h"

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsAnimationCurveKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	int32 Frame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;
};

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsAnimationCurve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAddKeyOnEachFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAlsAnimationCurveKey> Keys
	{
		{0, 0.0f}
	};
};

UCLASS(Meta = (DisplayName = "Als Create Curves Animation Modifier"))
class ALSEDITOR_API UAlsAnimationModifier_CreateCurves : public UAnimationModifier
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	TArray<FAlsAnimationCurve> Curves
	{
		{UAlsConstants::FootLeftIkCurve()},
		{UAlsConstants::FootLeftLockCurve()},
		{UAlsConstants::FootRightIkCurve()},
		{UAlsConstants::FootRightLockCurve()},
		{UAlsConstants::FootPlantedCurve()},
		{UAlsConstants::FeetCrossingCurve()},

		{UAlsConstants::PoseStandCurve()},
		{UAlsConstants::PoseCrouchCurve()},
		{UAlsConstants::PoseAirCurve()},

		{UAlsConstants::GaitTypeCurve()},
		{UAlsConstants::TransitionsAllowedCurve()},
		{UAlsConstants::SprintBlockCurve()},
		{UAlsConstants::GroundPredictionBlockCurve()},
		{UAlsConstants::FootstepSoundBlockCurve()}
	};

protected:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
