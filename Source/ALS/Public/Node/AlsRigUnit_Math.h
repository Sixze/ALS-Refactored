#pragma once

#include "Units/RigUnit.h"

#include "AlsRigUnit_Math.generated.h"

USTRUCT(Meta = (Abstract, NodeColor = "0.05 0.25 0.05"))
struct ALS_API FAlsRigUnit_MathBase : public FRigUnit
{
	GENERATED_BODY()
};

USTRUCT(Meta = (Abstract, NodeColor = "0.4 0.05 0.4"))
struct ALS_API FAlsRigUnit_HighLevelBaseMutable : public FRigUnitMutable
{
	GENERATED_BODY()
};

// Calculates the intersection location and direction of the perpendicular to line AC through point B.
USTRUCT(DisplayName = "Calculate Pole Vector", Meta = (Category = "ALS|Als", Varying))
struct ALS_API FAlsRigUnit_CalculatePoleVector : public FAlsRigUnit_MathBase
{
	GENERATED_BODY()

public:
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

public:
	RIGVM_METHOD()
	virtual void Execute(const FRigUnitContext& Context) override;
};

USTRUCT(DisplayName = "Hand Ik Retargeting", Meta = (Category = "ALS|Als"))
struct ALS_API FAlsRigUnit_HandIkRetargeting : public FAlsRigUnit_HighLevelBaseMutable
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey LeftHandBone;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey LeftHandIkBone;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey RightHandBone;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey RightHandIkBone;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	TArray<FRigElementKey> BonesToMove;

	// Which hand to favor. 0.5 is equal weight for both, 1 - right hand, 0 - left hand.
	UPROPERTY(Meta = (Input))
	float RetargetingWeight{0.5f};

	UPROPERTY(Meta = (Input))
	float Weight{1.0f};

	UPROPERTY(Meta = (Input, Constant))
	bool bPropagateToChildren{false};

public:
	RIGVM_METHOD()
	virtual void Execute(const FRigUnitContext& Context) override;
};
