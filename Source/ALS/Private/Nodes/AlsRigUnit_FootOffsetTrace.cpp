#include "Nodes/AlsRigUnit_FootOffsetTrace.h"

#include "Engine/HitResult.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRigUnit_FootOffsetTrace)

FAlsRigUnit_FootOffsetTrace_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (!bEnabled)
	{
		OffsetLocationZ = 0.0f;
		OffsetNormal = FVector::ZAxisVector;
		return;
	}

	// Trace downward from the foot location to find the geometry. If the surface is walkable, save the impact location and normal.

	const FVector TraceStart{FootTargetLocation.X, FootTargetLocation.Y, TraceDistanceUpward};
	const FVector TraceEnd{FootTargetLocation.X, FootTargetLocation.Y, -TraceDistanceDownward};

	FHitResult Hit;
	ExecuteContext.GetWorld()->LineTraceSingleByChannel(Hit, ExecuteContext.ToWorldSpace(TraceStart), ExecuteContext.ToWorldSpace(TraceEnd),
	                                                    TraceChannel, {__FUNCTION__, true, ExecuteContext.GetOwningActor()});

	auto* DrawInterface{ExecuteContext.GetDrawInterface()};
	if (DrawInterface != nullptr && bDrawDebug)
	{
		DrawInterface->DrawLine(FTransform::Identity, TraceStart, TraceEnd, {0.0f, 0.25f, 1.0f}, 1.0f);

		if (Hit.bBlockingHit)
		{
			DrawInterface->DrawPoint(FTransform::Identity, Hit.ImpactPoint, 12.0f, {0.0f, 0.75f, 1.0f});
		}
	}

	const auto HitNormal{ExecuteContext.GetToWorldSpaceTransform().InverseTransformVector(Hit.ImpactNormal)};

	if (!Hit.bBlockingHit || HitNormal.Z < FMath::Cos(FMath::DegreesToRadians(WalkableFloorAngle)))
	{
		OffsetLocationZ = 0.0f;
		OffsetNormal = FVector::ZAxisVector;
		return;
	}

	const auto HitLocation{ExecuteContext.ToVMSpace(Hit.ImpactPoint)};

	// Calculate how much we need to offset the foot along the Z axis to get it perfectly aligned with the sloped surface.
	// Without this, the foot will sink into the surface. This formula can be derived from the right triangle cosine formula
	// cos(a) = adjacent / hypotenuse, where cos(a) is SlopeAngleCos and adjacent is FootHeight. HitLocation.Z already contains
	// a correction for FootHeight, so we need to subtract the FootHeight at the end of the formula so it won't be applied twice.

	const auto SlopeAngleCos{UE_REAL_TO_FLOAT(HitNormal.Z)};
	const auto SlopeOffsetZ{SlopeAngleCos > UE_SMALL_NUMBER ? FootHeight / SlopeAngleCos - FootHeight : 0.0f};

	OffsetLocationZ = UE_REAL_TO_FLOAT(HitLocation.Z + SlopeOffsetZ);
	OffsetNormal = HitNormal;
}
