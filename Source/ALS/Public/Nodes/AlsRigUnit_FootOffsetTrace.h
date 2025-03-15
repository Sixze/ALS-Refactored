#pragma once

#include "Units/RigUnit.h"
#include "AlsRigUnit_FootOffsetTrace.generated.h"

USTRUCT(DisplayName = "Foot Offset Trace", Meta = (Category = "ALS", NodeColor = "0.2 0.4 1.0"))
struct ALS_API FAlsRigUnit_FootOffsetTrace : public FRigUnit
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, Meta = (Input))
	FVector FootTargetLocation{ForceInit};

	UPROPERTY(Meta = (Input))
	TEnumAsByte<ECollisionChannel> TraceChannel{ECC_Visibility};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float TraceDistanceUpward{50.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float TraceDistanceDownward{80.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ClampMax = 90, ForceUnits = "deg"))
	float WalkableFloorAngle{45.0f};

	UPROPERTY(Meta = (Input, ClampMin = 0, ForceUnits = "cm"))
	float FootHeight{13.5f};

	UPROPERTY(Meta = (Input))
	bool bEnabled{true};

	UPROPERTY(meta = (Input, DetailsOnly))
	bool bDrawDebug{false};

	UPROPERTY(Transient, Meta = (Output))
	float OffsetLocationZ{0.0f};

	UPROPERTY(Transient, Meta = (Output))
	FVector OffsetNormal{ForceInit};

public:
	RIGVM_METHOD()
	virtual void Execute() override;
};
