#pragma once

#include "Animation/AnimNodeBase.h"

#include "AlsAnimNode_CurvesBlend.generated.h"

UENUM(BlueprintType)
enum class EAlsCurvesBlendMode : uint8
{
	// Blend poses using blend amount.
	BlendByAmount,
	// Only set the value if the curves pose has the curve value.
	Combine,
	// Only set the value if the source pose doesn't have the curve value.
	CombinePreserved,
	// Find the highest curve value from multiple poses and use that.
	UseMaxValue,
	// Find the lowest curve value from multiple poses and use that.
	UseMinValue,
	// Completely override source pose.
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (PinShownByDefault))
	float BlendAmount{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EAlsCurvesBlendMode BlendMode{EAlsCurvesBlendMode::Override};

public:
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;

	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;

	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;

	virtual void Evaluate_AnyThread(FPoseContext& Output) override;

	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
};
