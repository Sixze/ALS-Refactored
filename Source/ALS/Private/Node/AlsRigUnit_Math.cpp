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
		CachedItemA.Reset();
		CachedItemB.Reset();
		CachedItemC.Reset();
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

	if (!CachedItemA.UpdateCache(ItemA, Hierarchy) ||
	    !CachedItemB.UpdateCache(ItemB, Hierarchy) ||
	    !CachedItemC.UpdateCache(ItemC, Hierarchy))
	{
		return;
	}

	if (!bInitial)
	{
		EndLocation = {Hierarchy->GetGlobalTransform(CachedItemB).GetLocation()};

		if (TryCalculatePoleVector(Hierarchy->GetGlobalTransform(CachedItemA).GetLocation(), EndLocation,
		                           Hierarchy->GetGlobalTransform(CachedItemC).GetLocation(), StartLocation, Direction))
		{
			bSuccess = true;
			return;
		}
	}

	StartLocation = FVector::ZeroVector;
	EndLocation = Hierarchy->GetInitialGlobalTransform(CachedItemB).GetLocation();
	Direction = FVector::ZeroVector;

	bSuccess = TryCalculatePoleVector(Hierarchy->GetInitialGlobalTransform(CachedItemA).GetLocation(), EndLocation,
	                                  Hierarchy->GetInitialGlobalTransform(CachedItemC).GetLocation(), StartLocation, Direction);
}

FAlsRigUnit_HandIkRetargeting_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!FAnimWeight::IsRelevant(Weight))
	{
		return;
	}

	if (Context.State == EControlRigState::Init)
	{
		CachedLeftHandBone.Reset();
		CachedLeftHandIkBone.Reset();
		CachedRightHandBone.Reset();
		CachedRightHandIkBone.Reset();
		CachedBonesToMove.Reset();
	}

	auto* Hierarchy{ExecuteContext.Hierarchy};
	if (Hierarchy == nullptr)
	{
		return;
	}

	if (!CachedLeftHandBone.UpdateCache(LeftHandBone, Hierarchy) ||
	    !CachedLeftHandIkBone.UpdateCache(LeftHandIkBone, Hierarchy) ||
	    !CachedRightHandBone.UpdateCache(RightHandBone, Hierarchy) ||
	    !CachedRightHandIkBone.UpdateCache(RightHandIkBone, Hierarchy))
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
		CachedBonesToMove.SetNum(CachedBonesToMove.Num());
	}

	for (auto i{0}; i < BonesToMove.Num(); i++)
	{
		if (Context.State == EControlRigState::Init)
		{
			CachedBonesToMove[i].Reset();
		}

		if (!CachedBonesToMove[i].UpdateCache(BonesToMove[i], Hierarchy))
		{
			continue;
		}

		auto BoneTransform{Hierarchy->GetGlobalTransform(CachedBonesToMove[i])};
		BoneTransform.AddToTranslation(RetargetingOffset);

		Hierarchy->SetGlobalTransform(CachedBonesToMove[i], BoneTransform, bPropagateToChildren);
	}
}
