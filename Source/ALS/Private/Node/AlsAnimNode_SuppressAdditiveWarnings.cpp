#include "Node/AlsAnimNode_SuppressAdditiveWarnings.h"

void FAlsAnimNode_SuppressAdditiveWarnings::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)

	Pose.Initialize(Context);
}

void FAlsAnimNode_SuppressAdditiveWarnings::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)

	Pose.CacheBones(Context);
}

void FAlsAnimNode_SuppressAdditiveWarnings::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)

	Pose.Update(Context);
}

void FAlsAnimNode_SuppressAdditiveWarnings::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)

	// Restrict this fix to Unreal Engine 5.0 or below. I hope in the future Epic Games will
	// fix this issue in the correct way and this temporary fix will no longer be needed.

#if ENGINE_MAJOR_VERSION <= 5 && ENGINE_MINOR_VERSION <= 0
	// Construct a new FPoseContext with bExpectsAdditivePose set to false to suppress "Trying to play a
	// non-additive animation into a pose that is expected to be additive" warnings when evaluating a pose.

	FPoseContext NewOutput{Output.AnimInstanceProxy, false};
	NewOutput.SetNodeIds(Output);

	Pose.Evaluate(NewOutput);

	Output.Pose = MoveTemp(NewOutput.Pose);
	Output.Curve = MoveTemp(NewOutput.Curve);
	Output.CustomAttributes = MoveTemp(NewOutput.CustomAttributes);
#else
	Pose.Evaluate(Output);
#endif
}

void FAlsAnimNode_SuppressAdditiveWarnings::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)

	Pose.GatherDebugData(DebugData);
}
