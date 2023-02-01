#pragma once

#include "Animation/AnimNodeBase.h"
#include "AlsAnimNode_CurvesBlend.generated.h"

UENUM(BlueprintType)
enum class EAlsCurvesBlendMode : uint8
{
	// Blend poses using blend amount. Same as ECurveBlendOption::BlendByWeight.
	BlendByAmount,
	// Only set the value if the curves pose has the curve value. Same as ECurveBlendOption::Override.
	Combine,
	// Only set the value if the source pose doesn't have the curve value. Same as ECurveBlendOption::DoNotOverride.
	CombinePreserved,
	// Find the highest curve value from multiple poses and use that. Same as ECurveBlendOption::UseMaxValue.
	UseMaxValue,
	// Find the lowest curve value from multiple poses and use that. Same as  ECurveBlendOption::UseMinValue.
	UseMinValue,
	// Completely override source pose. Same as ECurveBlendOption::UseBasePose.
	Override
};

USTRUCT(BlueprintInternalUseOnly)
struct ALS_API FAlsAnimNode_CurvesBlend : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPoseLink SourcePose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPoseLink CurvesPose;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 1, FoldProperty, PinShownByDefault))
	float BlendAmount{1.0f};

	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (FoldProperty))
	EAlsCurvesBlendMode BlendMode{EAlsCurvesBlendMode::BlendByAmount};
#endif

public:
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;

	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;

	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;

	virtual void Evaluate_AnyThread(FPoseContext& Output) override;

	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

public:
	float GetBlendAmount() const;

	EAlsCurvesBlendMode GetBlendMode() const;
};
