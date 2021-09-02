#include "ControlRig/AlsRigUnit_Math.h"

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
