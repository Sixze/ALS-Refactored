#include "Nodes/AlsAnimNode_SuppressAdditiveWarnings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNode_SuppressAdditiveWarnings)

void FAlsAnimNode_SuppressAdditiveWarnings::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)

	Super::Initialize_AnyThread(Context);

	Pose.Initialize(Context);
}

void FAlsAnimNode_SuppressAdditiveWarnings::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)

	Super::CacheBones_AnyThread(Context);

	Pose.CacheBones(Context);
}

void FAlsAnimNode_SuppressAdditiveWarnings::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)

	Super::Update_AnyThread(Context);

	Pose.Update(Context);
}

void FAlsAnimNode_SuppressAdditiveWarnings::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)

	Super::Evaluate_AnyThread(Output);

	// Construct a new FPoseContext with bExpectsAdditivePose set to false to suppress "Trying to play a
	// non-additive animation into a pose that is expected to be additive" warnings when evaluating a pose.

	// TODO Check the need for this temporary fix in future engine versions.

	FPoseContext NewOutput{Output.AnimInstanceProxy, false};
	NewOutput.SetNodeIds(Output);

	Pose.Evaluate(NewOutput);

	Output.Pose = MoveTemp(NewOutput.Pose);
	Output.Curve = MoveTemp(NewOutput.Curve);
	Output.CustomAttributes = MoveTemp(NewOutput.CustomAttributes);
}

void FAlsAnimNode_SuppressAdditiveWarnings::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)

	Pose.GatherDebugData(DebugData);
}
