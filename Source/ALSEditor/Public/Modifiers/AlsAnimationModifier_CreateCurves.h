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
	uint8 bAddKeyOnEachFrame : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TArray<FAlsAnimationCurveKey> Keys
	{
		{.Frame = 0, .Value = 0.0f}
	};
};

UCLASS(DisplayName = "Als Create Curves Animation Modifier")
class ALSEDITOR_API UAlsAnimationModifier_CreateCurves : public UAnimationModifier
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bOverrideExistingCurves : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FAlsAnimationCurve> Curves
	{
		{.Name = UAlsConstants::PoseGaitCurveName()},
		{.Name = UAlsConstants::PoseMovingCurveName()},
		{.Name = UAlsConstants::PoseStandingCurveName()},
		{.Name = UAlsConstants::PoseCrouchingCurveName()},
		{.Name = UAlsConstants::PoseInAirCurveName()},
		{.Name = UAlsConstants::PoseGroundedCurveName()},

		{.Name = UAlsConstants::FootLeftIkCurveName()},
		{.Name = UAlsConstants::FootLeftLockCurveName()},
		{.Name = UAlsConstants::FootRightIkCurveName()},
		{.Name = UAlsConstants::FootRightLockCurveName()},
		{.Name = UAlsConstants::FootPlantedCurveName()},
		{.Name = UAlsConstants::FeetCrossingCurveName()},

		{.Name = UAlsConstants::AllowTransitionsCurveName()},
		{.Name = UAlsConstants::SprintBlockCurveName()},
		{.Name = UAlsConstants::GroundPredictionBlockCurveName()},
		{.Name = UAlsConstants::FootstepSoundBlockCurveName()}
	};

public:
	virtual void OnApply_Implementation(UAnimSequence* Sequence) override;
};
