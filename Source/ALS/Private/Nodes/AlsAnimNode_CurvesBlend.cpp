#include "Nodes/AlsAnimNode_CurvesBlend.h"

#include "Animation/AnimTrace.h"
#include "Utility/AlsEnumUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNode_CurvesBlend)

void FAlsAnimNode_CurvesBlend::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::Initialize_AnyThread(Context);

	BasePose.Initialize(Context);
	CurvePose.Initialize(Context);
}

void FAlsAnimNode_CurvesBlend::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::CacheBones_AnyThread(Context);

	BasePose.CacheBones(Context);
	CurvePose.CacheBones(Context);
}

void FAlsAnimNode_CurvesBlend::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::Update_AnyThread(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	BasePose.Update(Context);

	if (FAnimWeight::IsRelevant(BlendAmount))
	{
		CurvePose.Update(Context.FractionalWeight(BlendAmount));
	}

	TRACE_ANIM_NODE_VALUE(Context, TEXT("Blend Amount"), BlendAmount)
	TRACE_ANIM_NODE_VALUE(Context, TEXT("Blend Mode"), *AlsEnumUtility::GetNameStringByValue(BlendMode))
}

void FAlsAnimNode_CurvesBlend::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(CurvesBlend, !IsInGameThread())

	Super::Evaluate_AnyThread(Output);

	BasePose.Evaluate(Output);

	if (!FAnimWeight::IsRelevant(BlendAmount))
	{
		return;
	}

	auto CurvesPoseContext{Output};
	CurvePose.Evaluate(CurvesPoseContext);

	switch (BlendMode)
	{
		case EAlsCurvesBlendMode::Accumulate:
			Output.Curve.Accumulate(CurvesPoseContext.Curve, BlendAmount);
			break;

		case EAlsCurvesBlendMode::Lerp:
			Output.Curve.LerpTo(CurvesPoseContext.Curve, BlendAmount);
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
			Output.Curve.Override(CurvesPoseContext.Curve, BlendAmount);
			break;

		default:
			break;
	}
}

void FAlsAnimNode_CurvesBlend::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	TStringBuilder<256> DebugItemBuilder{InPlace, DebugData.GetNodeName(this), TEXTVIEW(": Blend Amount: ")};

	DebugItemBuilder.Appendf(TEXT("%.2f"), BlendAmount);

	DebugData.AddDebugItem(FString{DebugItemBuilder});
	BasePose.GatherDebugData(DebugData.BranchFlow(1.0f));
	CurvePose.GatherDebugData(DebugData.BranchFlow(BlendAmount));
}
