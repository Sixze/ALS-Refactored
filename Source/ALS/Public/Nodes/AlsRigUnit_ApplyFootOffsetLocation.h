#pragma once

#include "Units/RigUnit.h"
#include "Utility/AlsMath.h"
#include "AlsRigUnit_ApplyFootOffsetLocation.generated.h"

USTRUCT(DisplayName = "Apply Foot Offset Location", Meta = (Category = "ALS", NodeColor = "0.0 0.36 1.0"))
struct ALS_API FAlsRigUnit_ApplyFootOffsetLocation : public FRigUnitMutable
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey PelvisItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey ThighItem;

	UPROPERTY(Transient, Meta = (Input))
	FVector FootTargetLocation{ForceInit};

	UPROPERTY(Transient, Meta = (Input))
	float FootOffsetLocationZ{0.0f};

	UPROPERTY(Transient, Meta = (Input))
	float PelvisOffset{0.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float LegLength{0.0f};

	/// Limits how high the foot can be raised relative to the pelvis.
	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float MinPelvisToFootDistanceZ{50.0f};

	/// Prevents legs from being fully straightened.
	UPROPERTY(Meta = (Input, ClampMin = 0.01, ForceUnits = "x"))
	float MaxLegStretchRatio{0.99f};

	/// Determines how hard the spring pulls towards the target. This value represents
	/// the frequency at which the spring oscillates when there is no damping.
	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "hz"))
	float OffsetInterpolationFrequency{12.0};

	/// If less than 1, the spring will oscillate before settling on the target.
	/// If equal to 1, the spring will reach the target without overshooting.
	/// If greater than 1, the spring will take longer to reach the target.
	UPROPERTY(Meta = (Input, ClampMin = 0))
	float OffsetInterpolationDampingRatio{2.0f};

	/// Amount of velocity passed to the spring. A value of 1 results in a more responsive output. However, if the input
	/// is noisy or has step changes, these discontinuities will be more noticeable than they would be with a lower value.
	UPROPERTY(Meta = (Input, ClampMin = 0, ClampMax = 1))
	float OffsetInterpolationTargetVelocityAmount{0.0f};

	UPROPERTY(Transient)
	bool bInitialized{false};

	UPROPERTY(Transient)
	FCachedRigElement CachedPelvisItem;

	UPROPERTY(Transient)
	FCachedRigElement CachedThighItem;

	UPROPERTY(Transient)
	FAlsSpringFloatState OffsetSpringState;

	UPROPERTY(Transient)
	float OffsetLocationZ{0.0f};

	UPROPERTY(Transient, Meta = (Output))
	FVector FootLocation{ForceInit};

public:
	virtual void Initialize() override;

	RIGVM_METHOD()
	// ReSharper disable once CppFunctionIsNotImplemented
	virtual void Execute() override;
};
