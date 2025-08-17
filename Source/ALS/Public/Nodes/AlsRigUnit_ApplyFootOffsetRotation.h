#pragma once

#include "Units/RigUnit.h"
#include "AlsRigUnit_ApplyFootOffsetRotation.generated.h"

USTRUCT(DisplayName = "Apply Foot Offset Rotation", Meta = (Category = "ALS", NodeColor = "0.0 0.36 1.0"))
struct ALS_API FAlsRigUnit_ApplyFootOffsetRotation : public FRigUnitMutable
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey CalfItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey FootItem;

	UPROPERTY(Transient, Meta = (Input))
	FQuat FootTargetRotation{ForceInit};

	UPROPERTY(Transient, Meta = (Input))
	FVector FootOffsetNormal{FVector::ZAxisVector};

	UPROPERTY(DisplayName = "Swing 1 Limit Angle", Meta = (Input, ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	FFloatInterval Swing1LimitAngle{-20.0f, 40.0f};

	UPROPERTY(DisplayName = "Swing 2 Limit Angle", Meta = (Input, ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	FFloatInterval Swing2LimitAngle{-15.0f, 5.0f};

	UPROPERTY(Meta = (Input, ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	FFloatInterval TwistLimitAngle{0.0f, 0.0f};

	// The lower the value, the faster the interpolation. A zero value results in instant interpolation.
	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "s"))
	float OffsetInterpolationHalfLife{0.1f};

	UPROPERTY(Transient)
	bool bInitialized{false};

	UPROPERTY(Transient)
	FCachedRigElement CachedCalfItem;

	UPROPERTY(Transient)
	FCachedRigElement CachedFootItem;

	UPROPERTY(Transient)
	FVector OffsetNormal{FVector::ZAxisVector};

	UPROPERTY(Transient, Meta = (Output))
	FQuat FootRotation{ForceInit};

	UPROPERTY(Transient)
	FQuat FootReferenceLocalRotation{ForceInit};

public:
	virtual void Initialize() override;

	RIGVM_METHOD()
	// ReSharper disable once CppFunctionIsNotImplemented
	virtual void Execute() override;
};
