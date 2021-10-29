#pragma once

#include "AnimationModifier.h"
#include "Utility/AlsConstants.h"

#include "AlsAnimationModifier_CreateCurves.generated.h"

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsAnimationCurveKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	int32 Frame{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value{0.0f};
};

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsAnimationCurve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAddKeyOnEachFrame{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAlsAnimationCurveKey> Keys
	{
		{0, 0.0f}
	};
};

UCLASS(DisplayName = "Als Create Curves Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CreateCurves : public UAnimationModifier
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	bool bOverrideExistingCurves;

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

		{UAlsConstants::GaitAmountCurve()},
		{UAlsConstants::AllowTransitionsCurve()},
		{UAlsConstants::SprintBlockCurve()},
		{UAlsConstants::GroundPredictionBlockCurve()},
		{UAlsConstants::FootstepSoundBlockCurve()}
	};

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
