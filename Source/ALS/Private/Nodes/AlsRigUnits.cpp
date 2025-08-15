#include "Nodes/AlsRigUnits.h"

#include "Engine/World.h"
#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnits)

FAlsRigVMFunction_Clamp01Float_Execute()
{
	Result = UAlsMath::Clamp01(Value);
}

void FAlsRigVMFunction_DamperExactVector::Initialize()
{
	bInitialized = false;
}

FAlsRigVMFunction_DamperExactVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!bInitialized)
	{
		bInitialized = true;
		Current = Target;
	}

	Current = UAlsMath::DamperExact(Current, Target, UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), HalfLife);
}

void FAlsRigVMFunction_DamperExactQuaternion::Initialize()
{
	bInitialized = false;
}

FAlsRigVMFunction_DamperExactQuaternion_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!bInitialized)
	{
		bInitialized = true;
		Current = Target;
	}

	Current = UAlsMath::DamperExact(Current, Target, UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), HalfLife);
}

FAlsRigUnit_CalculatePoleVector_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto* Hierarchy{ExecuteContext.Hierarchy};

	if (!IsValid(Hierarchy) ||
	    !CachedItemA.UpdateCache(ItemA, Hierarchy) ||
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

FAlsRigVMFunction_IsGameWorld_Execute()
{
	if (!BlockToRun.IsNone())
	{
		BlockToRun = ControlFlowCompletedName;
		return;
	}

#if WITH_EDITOR
	const auto* World{ExecuteContext.GetWorld()};

	BlockToRun = IsValid(World) && World->IsGameWorld()
		             ? FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAlsRigVMFunction_IsGameWorld, True)}
		             : FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAlsRigVMFunction_IsGameWorld, False)};
#else
	BlockToRun = FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAlsRigVMFunction_IsGameWorld, True)};
#endif
}

const TArray<FName>& FAlsRigVMFunction_IsGameWorld::GetControlFlowBlocks_Impl() const
{
	static const TArray<FName> Blocks{
		FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAlsRigVMFunction_IsGameWorld, True)},
		FName{GET_MEMBER_NAME_STRING_VIEW_CHECKED(FAlsRigVMFunction_IsGameWorld, False)},
		ForLoopCompletedPinName
	};

	return Blocks;
}
