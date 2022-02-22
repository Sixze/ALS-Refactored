#include "Node/AlsRigUnit_Math.h"

#include "Animation/AnimTypes.h"
#include "Units/RigUnitContext.h"

static bool TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation, const FVector& CLocation,
                                   FVector& StartLocation, FVector& Direction)
{
	auto AcVector{CLocation - ALocation};
	if (!AcVector.Normalize())
	{
		return false;
	}

	const auto AbVector{BLocation - ALocation};
	if (AbVector.IsNearlyZero())
	{
		return false;
	}

	StartLocation = ALocation + AbVector.ProjectOnToNormal(AcVector);
	Direction = (BLocation - StartLocation).GetSafeNormal();

	return true;
}

FAlsRigUnit_CalculatePoleVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (Context.State == EControlRigState::Init)
	{
		// CachedItemAIndex.Reset();
		// CachedItemBIndex.Reset();
		// CachedItemCIndex.Reset();
	}
	else if (bInitial)
	{
		return;
	}

	const auto* Hierarchy{Context.Hierarchy};
	if (Hierarchy == nullptr)
	{
		return;
	}

	// if (!CachedItemAIndex.UpdateCache(ItemA, Hierarchy) ||
	//     !CachedItemBIndex.UpdateCache(ItemB, Hierarchy) ||
	//     !CachedItemCIndex.UpdateCache(ItemC, Hierarchy))
	// {
	// 	return;
	// }

	if (!bInitial)
	{
		EndLocation = {Hierarchy->GetGlobalTransform(ItemB).GetLocation()};

		if (TryCalculatePoleVector(Hierarchy->GetGlobalTransform(ItemA).GetLocation(), EndLocation,
		                           Hierarchy->GetGlobalTransform(ItemC).GetLocation(), StartLocation, Direction))
		{
			bSuccess = true;
			return;
		}
	}

	StartLocation = FVector::ZeroVector;
	EndLocation = Hierarchy->GetInitialGlobalTransform(ItemB).GetLocation();
	Direction = FVector::ZeroVector;

	bSuccess = TryCalculatePoleVector(Hierarchy->GetInitialGlobalTransform(ItemA).GetLocation(), EndLocation,
	                                  Hierarchy->GetInitialGlobalTransform(ItemC).GetLocation(), StartLocation, Direction);
}

FAlsRigUnit_HandIkRetargeting_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!FAnimWeight::IsRelevant(Weight))
	{
		return;
	}

	auto* Hierarchy{ExecuteContext.Hierarchy};
	if (Hierarchy == nullptr)
	{
		return;
	}

	FVector RetargetingOffset;

	if (FAnimWeight::IsFullWeight(RetargetingWeight))
	{
		RetargetingOffset = Hierarchy->GetGlobalTransform(RightHandBone).GetLocation() -
		                    Hierarchy->GetGlobalTransform(RightHandIkBone).GetLocation();
	}
	else if (!FAnimWeight::IsRelevant(RetargetingWeight))
	{
		RetargetingOffset = Hierarchy->GetGlobalTransform(LeftHandBone).GetLocation() -
		                    Hierarchy->GetGlobalTransform(LeftHandIkBone).GetLocation();
	}
	else
	{
		RetargetingOffset = FMath::Lerp(Hierarchy->GetGlobalTransform(LeftHandBone).GetLocation(),
		                                Hierarchy->GetGlobalTransform(RightHandBone).GetLocation(),
		                                RetargetingWeight) -
		                    FMath::Lerp(Hierarchy->GetGlobalTransform(LeftHandIkBone).GetLocation(),
		                                Hierarchy->GetGlobalTransform(RightHandIkBone).GetLocation(),
		                                RetargetingWeight);
	}

	RetargetingOffset *= FMath::Min(1.0f, Weight);

	if (RetargetingOffset.IsNearlyZero())
	{
		return;
	}

	for (const auto& Bone : BonesToMove)
	{
		const auto BoneIndex{Hierarchy->GetIndex(Bone)};
		if (BoneIndex == INDEX_NONE)
		{
			continue;
		}

		auto BoneTransform{Hierarchy->GetGlobalTransform(Bone.Type, BoneIndex)};
		BoneTransform.AddToTranslation(RetargetingOffset);

		Hierarchy->SetGlobalTransform(Bone.Type, BoneIndex, BoneTransform, bPropagateToChildren);
	}
}
