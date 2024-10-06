#pragma once

#include "Units/RigUnit.h"
#include "AlsRigUnit_DistributeRotationSimple.generated.h"

USTRUCT(DisplayName = "Distribute Rotation Simple", Meta = (Category = "ALS", NodeColor = "0.0 0.36 1.0"))
struct ALS_API FAlsRigUnit_DistributeRotationSimple : public FRigUnitMutable
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input))
	TArray<FRigElementKey> Items;

	UPROPERTY(meta = (Input))
	FQuat Rotation{ForceInit};

	UPROPERTY(Transient)
	TArray<FCachedRigElement> CachedItems;

public:
	RIGVM_METHOD()
	virtual void Execute() override;
};
