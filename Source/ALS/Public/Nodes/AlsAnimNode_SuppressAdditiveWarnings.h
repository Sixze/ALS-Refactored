#pragma once

#include "Animation/AnimNodeBase.h"
#include "AlsAnimNode_SuppressAdditiveWarnings.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct ALS_API FAlsAnimNode_SuppressAdditiveWarnings : public FAnimNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPoseLink Pose;

public:
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;

	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;

	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;

	virtual void Evaluate_AnyThread(FPoseContext& Output) override;

	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
};
