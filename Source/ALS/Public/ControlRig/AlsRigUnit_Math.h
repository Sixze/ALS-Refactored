#pragma once

#include "Units/RigUnit.h"

#include "AlsRigUnit_Math.generated.h"

USTRUCT(Meta = (Abstract, NodeColor = "0.05 0.25 0.05"))
struct ALS_API FAlsRigUnit_MathBase : public FRigUnit
{
	GENERATED_BODY()
};

// Calculates the intersection location and direction of the perpendicular to line AC through point B
USTRUCT(Meta = (DisplayName = "Calculate Pole Vector", Category = "ALS|Als Math", Varying))
struct ALS_API FAlsRigUnit_CalculatePoleVector : public FAlsRigUnit_MathBase
{
	GENERATED_BODY()

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey ItemA;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey ItemB;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey ItemC;

	UPROPERTY(Meta = (Input))
	bool bInitial{false};

	UPROPERTY(Meta = (Output))
	bool bSuccess{false};

	UPROPERTY(Meta = (Output))
	FVector StartLocation{FVector::ZeroVector};

	UPROPERTY(Meta = (Output))
	FVector EndLocation{FVector::ZeroVector};

	UPROPERTY(Meta = (Output))
	FVector Direction{FVector::ForwardVector};

	// UPROPERTY()
	// FCachedRigElement CachedItemAIndex;
	//
	// UPROPERTY()
	// FCachedRigElement CachedItemBIndex;
	//
	// UPROPERTY()
	// FCachedRigElement CachedItemCIndex;

	RIGVM_METHOD()
	virtual void Execute(const FRigUnitContext& Context) override;
};
