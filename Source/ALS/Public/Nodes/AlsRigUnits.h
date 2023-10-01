#pragma once

#include "RigVMFunctions/Math/RigVMFunction_MathFloat.h"
#include "RigVMFunctions/Simulation/RigVMFunction_SimBase.h"
#include "Units/RigUnit.h"
#include "AlsRigUnits.generated.h"

USTRUCT(DisplayName="Clamp 01", Meta = (Category = "ALS"))
struct ALS_API FAlsRigVMFunction_Clamp01Float : public FRigVMFunction_MathFloatBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input))
	float Value{0.0f};

	UPROPERTY(Meta = (Output))
	float Result{0.0f};

public:
	RIGVM_METHOD()
	virtual void Execute() override;
};

USTRUCT(DisplayName = "Exponential Decay (Vector)", Meta = (Category = "ALS"))
struct ALS_API FAlsRigVMFunction_ExponentialDecayVector : public FRigVMFunction_SimBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input))
	FVector Target{ForceInit};

	UPROPERTY(Meta = (Input, ClampMin = 0))
	float Lambda{1.0f};

	UPROPERTY(Transient, Meta = (Output))
	FVector Current{ForceInit};

	UPROPERTY(Transient)
	bool bInitialized{false};

public:
	virtual void Initialize() override;

	RIGVM_METHOD()
	virtual void Execute() override;
};

// Calculates the projection location and direction of the perpendicular to AC through B.
USTRUCT(DisplayName = "Calculate Pole Vector", Meta = (Category = "ALS", NodeColor = "0.05 0.25 0.05"))
struct ALS_API FAlsRigUnit_CalculatePoleVector : public FRigUnit
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

	UPROPERTY(Transient, DisplayName = "Item B Location", Meta = (Output))
	FVector ItemBLocation{ForceInit};

	UPROPERTY(Transient, DisplayName = "Item B Projection Location", Meta = (Output))
	FVector ItemBProjectionLocation{ForceInit};

	UPROPERTY(Transient, Meta = (Output))
	FVector Direction{FVector::ForwardVector};

	UPROPERTY(Transient)
	bool bInitialized{false};

	UPROPERTY(Transient)
	FCachedRigElement CachedItemA;

	UPROPERTY(Transient)
	FCachedRigElement CachedItemB;

	UPROPERTY(Transient)
	FCachedRigElement CachedItemC;

public:
	virtual void Initialize() override;

	RIGVM_METHOD()
	virtual void Execute() override;
};

USTRUCT(DisplayName = "Hand Ik Retargeting", Meta = (Category = "ALS", NodeColor = "0 0.36 1.0"))
struct ALS_API FAlsRigUnit_HandIkRetargeting : public FRigUnitMutable
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
	bool bInitialized{false};

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
	virtual void Initialize() override;

	RIGVM_METHOD()
	virtual void Execute() override;
};
