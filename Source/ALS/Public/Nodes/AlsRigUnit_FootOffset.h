#pragma once

#include "Units/RigUnit.h"
#include "Utility/AlsMath.h"
#include "AlsRigUnit_FootOffset.generated.h"

USTRUCT(DisplayName = "Foot Offset", Meta = (Category = "ALS", NodeColor = "0 0.36 1.0"))
struct ALS_API FAlsRigUnit_FootOffset : public FRigUnitMutable
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, Meta = (Input))
	FVector Location{ForceInit};

	UPROPERTY(Transient, Meta = (Input))
	FQuat Rotation{ForceInit};

	UPROPERTY(Meta = (Input))
	TEnumAsByte<ECollisionChannel> TraceChannel{ECC_Visibility};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float TraceDistanceUpward{50.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float TraceDistanceDownward{45.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ClampMax = 90, ForceUnits = "deg"))
	float WalkableFloorAngle{45.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float FootHeight{13.5f};

	// Determines how hard the spring pulls towards the target. The value
	// represents the frequency at which it oscillates when there is no damping.
	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "hz"))
	float LocationInterpolationFrequency{0.4f};

	// If the value is less than 1, the spring will oscillate before stabilizing on the target.
	// If the value is equal to 1, the spring will reach the target without overshooting.
	// If the value is greater than 1, the spring will take longer to reach the target.
	UPROPERTY(Meta = (Input, ClampMin = 0))
	float LocationInterpolationDampingRatio{4.0f};

	// The amount of velocity that will be passed to the spring. A value of 1 will result in a more responsive output, but
	// if the input is noisy or has step changes, these discontinuities will be more noticeable than with a lower value.
	UPROPERTY(Meta = (Input, ClampMin = 0, ClampMax = 1))
	float LocationInterpolationTargetVelocityAmount{1.0f};

	// The higher the value, the faster the interpolation. A zero value will result in instant interpolation.
	UPROPERTY(Meta = (Input, ClampMin = 0))
	float RotationInterpolationSpeed{30.0f};

	UPROPERTY(Meta = (Input))
	bool bReset{false};

	UPROPERTY(Meta = (Input))
	bool bEnabled{true};

	UPROPERTY(meta = (Input, DetailsOnly))
	bool bDrawDebug{false};

	UPROPERTY(Transient)
	bool bInitialized{false};

	UPROPERTY(Transient, Meta = (Output))
	float OffsetTargetLocationZ{0.0f};

	UPROPERTY(Transient)
	FQuat OffsetTargetRotation{ForceInit};

	UPROPERTY(Transient)
	FAlsSpringFloatState OffsetSpringState;

	UPROPERTY(Transient)
	float OffsetLocationZ{0.0f};

	UPROPERTY(Transient)
	FQuat OffsetRotation{ForceInit};

	UPROPERTY(Transient, Meta = (Output))
	FVector ResultLocation{ForceInit};

	UPROPERTY(Transient, Meta = (Output))
	FQuat ResultRotation{ForceInit};

public:
	virtual void Initialize() override;

	RIGVM_METHOD()
	// ReSharper disable once CppFunctionIsNotImplemented
	virtual void Execute() override;
};
