#pragma once

#include "AnimationModifier.h"
#include "Utility/AlsConstants.h"
#include "AlsAnimationModifier_CreateCurves.generated.h"

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsAnimationCurveKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	int32 Frame{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	float Value{0.0f};
};

USTRUCT(BlueprintType)
struct ALSEDITOR_API FAlsAnimationCurve
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bAddKeyOnEachFrame{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TArray<FAlsAnimationCurveKey> Keys
	{
		{0, 0.0f}
	};
};

UCLASS(DisplayName = "Als Create Curves Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CreateCurves : public UAnimationModifier
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bOverrideExistingCurves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FAlsAnimationCurve> Curves
	{
		{UAlsConstants::PoseGaitCurve()},
		{UAlsConstants::PoseMovingCurve()},
		{UAlsConstants::PoseStandingCurve()},
		{UAlsConstants::PoseCrouchingCurve()},
		{UAlsConstants::PoseInAirCurve()},
		{UAlsConstants::PoseGroundedCurve()},

		{UAlsConstants::FootLeftIkCurve()},
		{UAlsConstants::FootLeftLockCurve()},
		{UAlsConstants::FootRightIkCurve()},
		{UAlsConstants::FootRightLockCurve()},
		{UAlsConstants::FootPlantedCurve()},
		{UAlsConstants::FeetCrossingCurve()},

		{UAlsConstants::AllowTransitionsCurve()},
		{UAlsConstants::SprintBlockCurve()},
		{UAlsConstants::GroundPredictionBlockCurve()},
		{UAlsConstants::FootstepSoundBlockCurve()}
	};

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
