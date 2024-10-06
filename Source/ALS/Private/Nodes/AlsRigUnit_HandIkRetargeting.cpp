#include "Nodes/AlsRigUnit_HandIkRetargeting.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_HandIkRetargeting)

FAlsRigUnit_HandIkRetargeting_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	auto* Hierarchy{ExecuteContext.Hierarchy};

	if (!IsValid(Hierarchy) ||
	    !CachedLeftHandItem.UpdateCache(LeftHandItem, Hierarchy) ||
	    !CachedLeftHandIkItem.UpdateCache(LeftHandIkItem, Hierarchy) ||
	    !CachedRightHandItem.UpdateCache(RightHandItem, Hierarchy) ||
	    !CachedRightHandIkItem.UpdateCache(RightHandIkItem, Hierarchy))
	{
		return;
	}

	if (!FAnimWeight::IsRelevant(Weight))
	{
		return;
	}

	FVector RetargetingOffset;

	if (FAnimWeight::IsFullWeight(RetargetingWeight))
	{
		RetargetingOffset = Hierarchy->GetGlobalTransform(CachedRightHandItem).GetLocation() -
		                    Hierarchy->GetGlobalTransform(CachedRightHandIkItem).GetLocation();
	}
	else if (!FAnimWeight::IsRelevant(RetargetingWeight))
	{
		RetargetingOffset = Hierarchy->GetGlobalTransform(CachedLeftHandItem).GetLocation() -
		                    Hierarchy->GetGlobalTransform(CachedLeftHandIkItem).GetLocation();
	}
	else
	{
		RetargetingOffset = FMath::Lerp(Hierarchy->GetGlobalTransform(CachedLeftHandItem).GetLocation(),
		                                Hierarchy->GetGlobalTransform(CachedRightHandItem).GetLocation(),
		                                RetargetingWeight) -
		                    FMath::Lerp(Hierarchy->GetGlobalTransform(CachedLeftHandIkItem).GetLocation(),
		                                Hierarchy->GetGlobalTransform(CachedRightHandIkItem).GetLocation(),
		                                RetargetingWeight);
	}

	RetargetingOffset *= FMath::Min(1.0f, Weight);

	if (RetargetingOffset.IsNearlyZero())
	{
		return;
	}

	if (CachedItemsToMove.Num() != ItemsToMove.Num())
	{
		CachedItemsToMove.Reset();
		CachedItemsToMove.SetNum(ItemsToMove.Num());
	}

	for (auto i{0}; i < ItemsToMove.Num(); i++)
	{
		if (CachedItemsToMove[i].UpdateCache(ItemsToMove[i], Hierarchy))
		{
			auto ItemTransform{Hierarchy->GetGlobalTransform(CachedItemsToMove[i])};
			ItemTransform.AddToTranslation(RetargetingOffset);

			Hierarchy->SetGlobalTransform(CachedItemsToMove[i], ItemTransform, bPropagateToChildren);
		}
	}
}
