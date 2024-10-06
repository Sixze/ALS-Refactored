#pragma once

#include "Units/RigUnit.h"
#include "AlsRigUnit_ChainLength.generated.h"

USTRUCT(DisplayName = "Chain Length", Meta = (Category = "ALS", NodeColor = "0.05 0.25 0.05"))
struct ALS_API FAlsRigUnit_ChainLength : public FRigUnit
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey AncestorItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey DescendantItem;

	UPROPERTY(Meta = (Input))
	bool bInitial{false};

	UPROPERTY(Transient, Meta = (Output))
	float Length{0.0f};

	UPROPERTY(Transient)
	FCachedRigElement CachedAncestorItem;

	UPROPERTY(Transient)
	FCachedRigElement CachedDescendantItem;

public:
	RIGVM_METHOD()
	virtual void Execute() override;
};
