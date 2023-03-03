#pragma once

#include "Units/RigUnit.h"
#include "AlsRigUnits.generated.h"

USTRUCT(Meta = (Abstract, NodeColor = "0.05 0.25 0.05"))
struct ALS_API FAlsRigUnit_MathBase : public FRigUnit
{
	GENERATED_BODY()
};

USTRUCT(Meta = (Abstract, NodeColor = "0.46 1.0 0.33"))
struct ALS_API FAlsRigUnit_HighLevelBase : public FRigUnitMutable
{
	GENERATED_BODY()
};

USTRUCT(Meta = (Abstract, NodeColor = "0.25 0.05 0.05"))
struct ALS_API FAlsRigUnit_SimulationBase : public FRigUnit
{
	GENERATED_BODY()
};

// Calculates the intersection location and direction of the perpendicular to AC through B.
USTRUCT(DisplayName = "Calculate Pole Vector", Meta = (Category = "ALS"))
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

	UPROPERTY(Transient, Meta = (Output))
	bool bSuccess{false};

	UPROPERTY(Transient, Meta = (Output))
	FVector StartLocation{ForceInit};

	UPROPERTY(Transient, Meta = (Output))
	FVector EndLocation{ForceInit};

	UPROPERTY(Transient, Meta = (Output))
	FVector Direction{FVector::ForwardVector};

	UPROPERTY(Transient)
	FCachedRigElement CachedItemA;

	UPROPERTY(Transient)
	FCachedRigElement CachedItemB;

	UPROPERTY(Transient)
	FCachedRigElement CachedItemC;

public:
	RIGVM_METHOD()
	virtual void Execute(const FRigUnitContext& Context) override;
};

USTRUCT(DisplayName = "Exponential Decay (Vector)", Meta = (Category = "ALS"))
struct ALS_API FAlsRigUnit_ExponentialDecayVector : public FAlsRigUnit_SimulationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input))
	FVector Target{ForceInit};

	UPROPERTY(Meta = (Input, ClampMin = 0))
	float Lambda{1.0f};

	UPROPERTY(Transient, Meta = (Output))
	FVector Current{ForceInit};

public:
	RIGVM_METHOD()
	virtual void Execute(const FRigUnitContext& Context) override;
};

USTRUCT(DisplayName = "Hand Ik Retargeting", Meta = (Category = "ALS"))
struct ALS_API FAlsRigUnit_HandIkRetargeting : public FAlsRigUnit_HighLevelBase
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

	UPROPERTY(Transient)
	FCachedRigElement CachedLeftHandBone;

	UPROPERTY(Transient)
	FCachedRigElement CachedLeftHandIkBone;

	UPROPERTY(Transient)
	FCachedRigElement CachedRightHandBone;

	UPROPERTY(Transient)
	FCachedRigElement CachedRightHandIkBone;

	UPROPERTY(Transient)
	TArray<FCachedRigElement> CachedBonesToMove;

public:
	RIGVM_METHOD()
	virtual void Execute(const FRigUnitContext& Context) override;
};
