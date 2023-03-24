#include "Nodes/AlsRigUnits.h"

#include "Units/RigUnitContext.h"
#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnits)

static bool TryCalculatePoleVector(const FVector& ALocation, const FVector& BLocation, const FVector& CLocation,
                                   FVector& ProjectionLocation, FVector& Direction)
{
	auto AcVector{CLocation - ALocation};
	auto AbVector{BLocation - ALocation};

	if (!AcVector.Normalize())
	{
		if (!AbVector.Normalize())
		{
			return false;
		}

		ProjectionLocation = ALocation;
		Direction = AbVector;

		return true;
	}

	if (AbVector.IsNearlyZero())
	{
		return false;
	}

	ProjectionLocation = ALocation + AbVector.ProjectOnToNormal(AcVector);
	Direction = (BLocation - ProjectionLocation).GetSafeNormal();

	return true;
}

FAlsRigUnit_ExponentialDecayVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	Current = (!bIsInitialized)
		          ? Target
		          : UAlsMath::ExponentialDecay(Current, Target, ExecuteContext.GetDeltaTime(), Lambda);
}

FAlsRigUnit_CalculatePoleVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;
	if (!IsValid(Hierarchy))
	{
		return;
	}

	if (!bIsInitialized)
	{
		CachedItemA.Reset();
		CachedItemB.Reset();
		CachedItemC.Reset();
	}
	else if (bInitial)
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
		const auto NewEndLocation{Hierarchy->GetGlobalTransform(CachedItemB).GetLocation()};

		if (TryCalculatePoleVector(Hierarchy->GetGlobalTransform(CachedItemA).GetLocation(), NewEndLocation,
		                           Hierarchy->GetGlobalTransform(CachedItemC).GetLocation(), StartLocation, Direction))
		{
			EndLocation = NewEndLocation;
			bSuccess = true;
			return;
		}
	}

	const auto NewEndLocation{Hierarchy->GetInitialGlobalTransform(CachedItemB).GetLocation()};

	if (TryCalculatePoleVector(Hierarchy->GetInitialGlobalTransform(CachedItemA).GetLocation(), NewEndLocation,
	                           Hierarchy->GetInitialGlobalTransform(CachedItemC).GetLocation(), StartLocation, Direction))
	{
		EndLocation = NewEndLocation;
		bSuccess = true;
		return;
	}

	bSuccess = false;
}

FAlsRigUnit_HandIkRetargeting_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	auto* Hierarchy{ExecuteContext.Hierarchy};
	if (!IsValid(Hierarchy))
	{
		return;
	}

	if (!bIsInitialized)
	{
		CachedLeftHandBone.Reset();
		CachedLeftHandIkBone.Reset();
		CachedRightHandBone.Reset();
		CachedRightHandIkBone.Reset();
		CachedBonesToMove.Reset();
		return;
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
		CachedBonesToMove.SetNum(CachedBonesToMove.Num());
	}

	for (auto i{0}; i < BonesToMove.Num(); i++)
	{
		if (!bIsInitialized)
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

void FAlsRigUnit_CalculatePoleVector::Initialize() {
	bIsInitialized = true;
	FAlsRigUnit_MathBase::Initialize();
}

void FAlsRigUnit_ExponentialDecayVector::Initialize() {
	bIsInitialized = true;
	FAlsRigUnit_SimulationBase::Initialize();
}

void FAlsRigUnit_HandIkRetargeting::Initialize() {
	bIsInitialized = true;
	FAlsRigUnit_HighLevelBase::Initialize();
}
