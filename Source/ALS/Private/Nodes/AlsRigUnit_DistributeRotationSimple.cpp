#include "Nodes/AlsRigUnit_DistributeRotationSimple.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_DistributeRotationSimple)

FAlsRigUnit_DistributeRotationSimple_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	auto* Hierarchy{ExecuteContext.Hierarchy};
	if (!IsValid(Hierarchy))
	{
		return;
	}

	if (Items.IsEmpty() || Rotation.IsIdentity(UE_KINDA_SMALL_NUMBER))
	{
		return;
	}

	const auto DeltaRotation{FQuat::Slerp(FQuat::Identity, Rotation, 1.0f / static_cast<float>(Items.Num()))};
	if (DeltaRotation.IsIdentity(UE_KINDA_SMALL_NUMBER))
	{
		return;
	}

	if (CachedItems.Num() != Items.Num())
	{
		CachedItems.Reset();
		CachedItems.SetNum(Items.Num());
	}

	for (auto i{0}; i < Items.Num(); i++)
	{
		if (CachedItems[i].UpdateCache(Items[i], Hierarchy))
		{
			auto NewTransform{Hierarchy->GetGlobalTransform(CachedItems[i])};
			NewTransform.SetRotation(DeltaRotation * NewTransform.GetRotation());

			Hierarchy->SetGlobalTransform(CachedItems[i], NewTransform);
		}
	}
}
