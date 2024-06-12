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
