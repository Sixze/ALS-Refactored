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
		bInitialized = true;
		Current = Target;
	}

	Current = UAlsMath::ExponentialDecay(Current, Target, UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), Lambda);
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
		bInitialized = true;

		CachedItemA.Reset();
		CachedItemB.Reset();
		CachedItemC.Reset();
	}

	if (!CachedItemA.UpdateCache(ItemA, Hierarchy) ||
	    !CachedItemB.UpdateCache(ItemB, Hierarchy) ||
	    !CachedItemC.UpdateCache(ItemC, Hierarchy))
	{
		return;
	}

	const auto NewItemBLocation{Hierarchy->GetGlobalTransformByIndex(CachedItemB, bInitial).GetLocation()};
	FVector NewItemBProjectionLocation;
	FVector NewPoleDirection;

	if (!UAlsMath::TryCalculatePoleVector(Hierarchy->GetGlobalTransformByIndex(CachedItemA, bInitial).GetLocation(), NewItemBLocation,
	                                      Hierarchy->GetGlobalTransformByIndex(CachedItemC, bInitial).GetLocation(),
	                                      NewItemBProjectionLocation, NewPoleDirection))
	{
		// Reuse the last successful result if a new pole vector can't be calculated.
		bSuccess = false;
		return;
	}

	ItemBLocation = NewItemBLocation;
	ItemBProjectionLocation = NewItemBProjectionLocation;
	PoleDirection = NewPoleDirection;
	bSuccess = true;
}
