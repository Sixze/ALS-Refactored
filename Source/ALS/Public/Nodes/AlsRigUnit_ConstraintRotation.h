#pragma once

#include "RigVMFunctions/Math/RigVMFunction_MathQuaternion.h"
#include "AlsRigUnit_ConstraintRotation.generated.h"

USTRUCT(DisplayName = "Constraint Rotation", Meta = (Category = "ALS"))
struct ALS_API FAlsRigUnit_ConstraintRotation : public FRigVMFunction_MathQuaternionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, Meta = (Input))
	FQuat CurrentRotation{ForceInit};

	UPROPERTY(Transient, Meta = (Input))
	FQuat TargetRotation{ForceInit};

	UPROPERTY(Meta = (Input, ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float TwistLimitAngle{180.0f};

	UPROPERTY(Meta = (Input))
	FQuat SwingLimitOffset{ForceInit};

	UPROPERTY(DisplayName = "Swing 1 Limit Angle", Meta = (Input, ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float Swing1LimitAngle{180.0f};

	UPROPERTY(DisplayName = "Swing 2 Limit Angle", Meta = (Input, ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float Swing2LimitAngle{180.0f};

	UPROPERTY(Transient, Meta = (Output))
	FQuat ConstrainedRotation{ForceInit};

public:
	RIGVM_METHOD()
	// ReSharper disable once CppFunctionIsNotImplemented
	virtual void Execute() override;
};
