#include "Nodes/AlsRigUnits.h"

#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnits)

FAlsRigVMFunction_Clamp01Float_Execute()
{
	Result = UAlsMath::Clamp01(Value);
}

void FAlsRigVMFunction_ExponentialDecayVector::Initialize()
{
	bInitialized = false;
}

FAlsRigVMFunction_ExponentialDecayVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!bInitialized)
	{
		Current = Target;

		bInitialized = true;
	}

	Current = UAlsMath::ExponentialDecay(Current, Target, ExecuteContext.GetDeltaTime(), Lambda);
}

void FAlsRigUnit_CalculatePoleVector::Initialize()
{
	bInitialized = false;
}

FAlsRigUnit_CalculatePoleVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto* Hierarchy{ExecuteContext.Hierarchy};
	if (!IsValid(Hierarchy))
	{
		return;
	}

	if (!bInitialized)
	{
		CachedItemA.Reset();
		CachedItemB.Reset();
		CachedItemC.Reset();

		bInitialized = true;
	}

	if (!CachedItemA.UpdateCache(ItemA, Hierarchy) ||
	    !CachedItemB.UpdateCache(ItemB, Hierarchy) ||
	    !CachedItemC.UpdateCache(ItemC, Hierarchy))
	{
		return;
	}

	const auto NewItemBLocation{Hierarchy->GetGlobalTransformByIndex(CachedItemB, bInitial).GetLocation()};
	FVector NewItemBProjectionLocation;
	FVector NewDirection;

	if (!UAlsMath::TryCalculatePoleVector(Hierarchy->GetGlobalTransformByIndex(CachedItemA, bInitial).GetLocation(), NewItemBLocation,
	                                      Hierarchy->GetGlobalTransformByIndex(CachedItemC, bInitial).GetLocation(),
	                                      NewItemBProjectionLocation, NewDirection))
	{
		// Reuse the last successful result if a new pole vector can't be calculated.
		bSuccess = false;
		return;
	}

	ItemBLocation = NewItemBLocation;
	ItemBProjectionLocation = NewItemBProjectionLocation;
	Direction = NewDirection;
	bSuccess = true;
}

void FAlsRigUnit_HandIkRetargeting::Initialize()
{
	bInitialized = false;
}

FAlsRigUnit_HandIkRetargeting_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	auto* Hierarchy{ExecuteContext.Hierarchy};
	if (!IsValid(Hierarchy))
	{
		return;
	}

	if (!bInitialized)
	{
		CachedLeftHandBone.Reset();
		CachedLeftHandIkBone.Reset();
		CachedRightHandBone.Reset();
		CachedRightHandIkBone.Reset();
		CachedBonesToMove.Reset();

		for (auto& Bone : CachedBonesToMove)
		{
			Bone.Reset();
		}

		bInitialized = true;
	}

	if (!CachedLeftHandBone.UpdateCache(LeftHandBone, Hierarchy) ||
	    !CachedLeftHandIkBone.UpdateCache(LeftHandIkBone, Hierarchy) ||
	    !CachedRightHandBone.UpdateCache(RightHandBone, Hierarchy) ||
	    !CachedRightHandIkBone.UpdateCache(RightHandIkBone, Hierarchy))
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
		RetargetingOffset = Hierarchy->GetGlobalTransform(CachedRightHandBone).GetLocation() -
		                    Hierarchy->GetGlobalTransform(CachedRightHandIkBone).GetLocation();
	}
	else if (!FAnimWeight::IsRelevant(RetargetingWeight))
	{
		RetargetingOffset = Hierarchy->GetGlobalTransform(CachedLeftHandBone).GetLocation() -
		                    Hierarchy->GetGlobalTransform(CachedLeftHandIkBone).GetLocation();
	}
	else
	{
		RetargetingOffset = FMath::Lerp(Hierarchy->GetGlobalTransform(CachedLeftHandBone).GetLocation(),
		                                Hierarchy->GetGlobalTransform(CachedRightHandBone).GetLocation(),
		                                RetargetingWeight) -
		                    FMath::Lerp(Hierarchy->GetGlobalTransform(CachedLeftHandIkBone).GetLocation(),
		                                Hierarchy->GetGlobalTransform(CachedRightHandIkBone).GetLocation(),
		                                RetargetingWeight);
	}

	RetargetingOffset *= FMath::Min(1.0f, Weight);

	if (RetargetingOffset.IsNearlyZero())
	{
		return;
	}

	if (CachedBonesToMove.Num() != BonesToMove.Num())
	{
		CachedBonesToMove.Reset();
		CachedBonesToMove.SetNum(BonesToMove.Num());
	}

	for (auto i{0}; i < BonesToMove.Num(); i++)
	{
		if (CachedBonesToMove[i].UpdateCache(BonesToMove[i], Hierarchy))
		{
			auto BoneTransform{Hierarchy->GetGlobalTransform(CachedBonesToMove[i])};
			BoneTransform.AddToTranslation(RetargetingOffset);

			Hierarchy->SetGlobalTransform(CachedBonesToMove[i], BoneTransform, bPropagateToChildren);
		}
	}
}
