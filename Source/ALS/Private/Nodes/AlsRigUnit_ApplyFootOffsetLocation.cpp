#include "Nodes/AlsRigUnit_ApplyFootOffsetLocation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_ApplyFootOffsetLocation)

void FAlsRigUnit_ApplyFootOffsetLocation::Initialize()
{
	bInitialized = false;
}

FAlsRigUnit_ApplyFootOffsetLocation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const auto* Hierarchy{ExecuteContext.Hierarchy};

	if (!IsValid(Hierarchy) ||
	    !CachedPelvisItem.UpdateCache(PelvisItem, Hierarchy) ||
	    !CachedThighItem.UpdateCache(ThighItem, Hierarchy))
	{
		return;
	}

	const auto PelvisLocationZ{UE_REAL_TO_FLOAT(Hierarchy->GetGlobalTransform(CachedPelvisItem).GetLocation().Z)};
	const auto ThighLocation{Hierarchy->GetGlobalTransform(CachedThighItem).GetLocation()};

	const auto MaxFootLocationZ{PelvisLocationZ - MinPelvisToFootDistanceZ};
	const auto MaxFootOffsetLocationZ{MaxFootLocationZ - UE_REAL_TO_FLOAT(FootTargetLocation.Z)};

	// Limit how high the foot offset can raise the foot relative to the pelvis.
	auto TargetOffsetLocationZ{FMath::Min(FootOffsetLocationZ, MaxFootOffsetLocationZ)};

	// Do not allow the foot offset to be lower than the pelvis offset to prevent leg stretching.
	TargetOffsetLocationZ = FMath::Max(TargetOffsetLocationZ, PelvisOffset);

	if (!bInitialized)
	{
		bInitialized = true;

		OffsetSpringState.Reset();
		OffsetLocationZ = TargetOffsetLocationZ;
	}
	else
	{
		OffsetLocationZ = UAlsMath::SpringDamperFloat(OffsetSpringState, OffsetLocationZ, TargetOffsetLocationZ,
		                                              UE_REAL_TO_FLOAT(ExecuteContext.GetDeltaTime()), OffsetInterpolationFrequency,
		                                              OffsetInterpolationDampingRatio, OffsetInterpolationTargetVelocityAmount);
	}

	FootLocation.X = FootTargetLocation.X;
	FootLocation.Y = FootTargetLocation.Y;
	FootLocation.Z = FootTargetLocation.Z + OffsetLocationZ;

	// Prevent the leg from being fully straightened. We do this after offset interpolation, otherwise the effect will not be noticeable.

	const auto LegVector{FootLocation - ThighLocation};
	const auto ClampedLegVector{LegVector.GetClampedToMaxSize(LegLength * MaxLegStretchRatio)};

	FootLocation = ThighLocation + ClampedLegVector;
}
