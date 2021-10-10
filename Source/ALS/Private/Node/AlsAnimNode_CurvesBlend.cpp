#include "Node/AlsAnimNode_CurvesBlend.h"

#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

void FAlsAnimNode_CurvesBlend::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)

	Super::Initialize_AnyThread(Context);

	SourcePose.Initialize(Context);
	CurvesPose.Initialize(Context);
}

void FAlsAnimNode_CurvesBlend::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)

	Super::CacheBones_AnyThread(Context);

	SourcePose.CacheBones(Context);
	CurvesPose.CacheBones(Context);
}

void FAlsAnimNode_CurvesBlend::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)

	Super::Update_AnyThread(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	SourcePose.Update(Context);

	if (FAnimWeight::IsRelevant(BlendAmount))
	{
		CurvesPose.Update(Context);
	}
}

void FAlsAnimNode_CurvesBlend::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)

	auto SourcePoseContext{Output};
	SourcePose.Evaluate(SourcePoseContext);

	Output = SourcePoseContext;

	if (!FAnimWeight::IsRelevant(BlendAmount))
	{
		return;
	}

	auto CurvesPoseContext{Output};
	CurvesPose.Evaluate(CurvesPoseContext);

	switch (BlendMode)
	{
		case EAlsCurvesBlendMode::BlendByAmount:
			Output.Curve.Accumulate(CurvesPoseContext.Curve, BlendAmount);
			break;

		case EAlsCurvesBlendMode::Combine:
			Output.Curve.Combine(CurvesPoseContext.Curve);
			break;

		case EAlsCurvesBlendMode::CombinePreserved:
			Output.Curve.CombinePreserved(CurvesPoseContext.Curve);
			break;

		case EAlsCurvesBlendMode::UseMaxValue:
			Output.Curve.UseMaxValue(CurvesPoseContext.Curve);
			break;

		case EAlsCurvesBlendMode::UseMinValue:
			Output.Curve.UseMinValue(CurvesPoseContext.Curve);
			break;

		case EAlsCurvesBlendMode::Override:
			Output.Curve.Override(CurvesPoseContext.Curve);
			break;
	}
}

void FAlsAnimNode_CurvesBlend::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)

	SourcePose.GatherDebugData(DebugData.BranchFlow(1.0f));
	CurvesPose.GatherDebugData(DebugData.BranchFlow(BlendAmount));
}
