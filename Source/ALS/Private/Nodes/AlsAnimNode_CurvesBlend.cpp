#include "Nodes/AlsAnimNode_CurvesBlend.h"

#include "Animation/AnimTrace.h"
#include "Utility/AlsEnumUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNode_CurvesBlend)

void FAlsAnimNode_CurvesBlend::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::Initialize_AnyThread(Context);

	SourcePose.Initialize(Context);
	CurvesPose.Initialize(Context);
}

void FAlsAnimNode_CurvesBlend::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::CacheBones_AnyThread(Context);

	SourcePose.CacheBones(Context);
	CurvesPose.CacheBones(Context);
}

void FAlsAnimNode_CurvesBlend::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()

	Super::Update_AnyThread(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	SourcePose.Update(Context);

	const auto CurrentBlendAmount{GetBlendAmount()};
	if (FAnimWeight::IsRelevant(CurrentBlendAmount))
	{
		CurvesPose.Update(Context);
	}

	TRACE_ANIM_NODE_VALUE(Context, TEXT("Blend Amount"), CurrentBlendAmount);
	TRACE_ANIM_NODE_VALUE(Context, TEXT("Blend Mode"), *AlsEnumUtility::GetNameStringByValue(GetBlendMode()));
}

void FAlsAnimNode_CurvesBlend::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	ANIM_MT_SCOPE_CYCLE_COUNTER_VERBOSE(CurvesBlend, !IsInGameThread());

	Super::Evaluate_AnyThread(Output);

	SourcePose.Evaluate(Output);

	const auto CurrentBlendAmount{GetBlendAmount()};
	if (!FAnimWeight::IsRelevant(CurrentBlendAmount))
	{
		return;
	}

	auto CurvesPoseContext{Output};
	CurvesPose.Evaluate(CurvesPoseContext);

	switch (GetBlendMode())
	{
		case EAlsCurvesBlendMode::BlendByAmount:
			Output.Curve.Accumulate(CurvesPoseContext.Curve, CurrentBlendAmount);
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

	TStringBuilder<256> DebugItemBuilder;

	DebugItemBuilder << DebugData.GetNodeName(this) << TEXTVIEW(": Blend Amount: ");
	DebugItemBuilder.Appendf(TEXT("%.2f"), GetBlendAmount());

	DebugData.AddDebugItem(FString{DebugItemBuilder});
	SourcePose.GatherDebugData(DebugData.BranchFlow(1.0f));
	CurvesPose.GatherDebugData(DebugData.BranchFlow(GetBlendAmount()));
}

float FAlsAnimNode_CurvesBlend::GetBlendAmount() const
{
	return GET_ANIM_NODE_DATA(float, BlendAmount);
}

EAlsCurvesBlendMode FAlsAnimNode_CurvesBlend::GetBlendMode() const
{
	return GET_ANIM_NODE_DATA(EAlsCurvesBlendMode, BlendMode);
}
