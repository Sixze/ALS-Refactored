#pragma once

#include "Units/RigUnit.h"
#include "AlsRigUnit_HandIkRetargeting.generated.h"

USTRUCT(DisplayName = "Hand Ik Retargeting", Meta = (Category = "ALS", NodeColor = "0.0 0.36 1.0"))
struct ALS_API FAlsRigUnit_HandIkRetargeting : public FRigUnitMutable
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey LeftHandItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey LeftHandIkItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey RightHandItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	FRigElementKey RightHandIkItem;

	UPROPERTY(Meta = (Input, ExpandByDefault))
	TArray<FRigElementKey> ItemsToMove;

	// Which hand to favor. 0.5 is equal weight for both, 1 - right hand, 0 - left hand.
	UPROPERTY(Meta = (Input))
	float RetargetingWeight{0.5f};

	UPROPERTY(Meta = (Input))
	float Weight{1.0f};

	UPROPERTY(Meta = (Input, Constant))
	bool bPropagateToChildren{false};

	UPROPERTY(Transient)
	FCachedRigElement CachedLeftHandItem;

	UPROPERTY(Transient)
	FCachedRigElement CachedLeftHandIkItem;

	UPROPERTY(Transient)
	FCachedRigElement CachedRightHandItem;

	UPROPERTY(Transient)
	FCachedRigElement CachedRightHandIkItem;

	UPROPERTY(Transient)
	TArray<FCachedRigElement> CachedItemsToMove;

public:
	RIGVM_METHOD()
	virtual void Execute() override;
};
