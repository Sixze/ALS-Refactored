#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "AlsConstants.generated.h"

UCLASS(meta = (BlueprintThreadSafe))
class ALS_API UAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Bones

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName RootBone()
	{
		return TEXT("root");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName PelvisBone()
	{
		return TEXT("pelvis");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName Spine03Bone()
	{
		return TEXT("spine_03");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName GunLeftVirtualBone()
	{
		return TEXT("VB ik_hand_gun_l");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName GunRightVirtualBone()
	{
		return TEXT("VB ik_hand_gun_r");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName FootLeftVirtualBone()
	{
		return TEXT("VB ik_foot_l");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName FootRightVirtualBone()
	{
		return TEXT("VB ik_foot_r");
	}

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static FName TransitionSlot()
	{
		return TEXT("Transition");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static FName TurnInPlaceSlot()
	{
		return TEXT("TurnInPlace");
	}

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHeadCurve()
	{
		return TEXT("LayerHead");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHeadAdditiveCurve()
	{
		return TEXT("LayerHeadAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmLeftCurve()
	{
		return TEXT("LayerArmLeft");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmLeftAdditiveCurve()
	{
		return TEXT("LayerArmLeftAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmLeftLocalSpaceCurve()
	{
		return TEXT("LayerArmLeftLocalSpace");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmRightCurve()
	{
		return TEXT("LayerArmRight");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmRightAdditiveCurve()
	{
		return TEXT("LayerArmRightAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmRightLocalSpaceCurve()
	{
		return TEXT("LayerArmRightLocalSpace");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHandLeftCurve()
	{
		return TEXT("LayerHandLeft");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHandRightCurve()
	{
		return TEXT("LayerHandRight");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerSpineCurve()
	{
		return TEXT("LayerSpine");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerSpineAdditiveCurve()
	{
		return TEXT("LayerSpineAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerPelvisCurve()
	{
		return TEXT("LayerPelvis");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerLegsCurve()
	{
		return TEXT("LayerLegs");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName HandLeftIkCurve()
	{
		return TEXT("HandLeftIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName HandRightIkCurve()
	{
		return TEXT("HandRightIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName AimManualCurve()
	{
		return TEXT("AimManual");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName AimBlockCurve()
	{
		return TEXT("AimBlock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName HipsDirectionLockCurve()
	{
		return TEXT("HipsDirectionLock");
	}

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootLeftIkCurve()
	{
		return TEXT("FootLeftIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootLeftLockCurve()
	{
		return TEXT("FootLeftLock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootRightIkCurve()
	{
		return TEXT("FootRightIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootRightLockCurve()
	{
		return TEXT("FootRightLock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootPlantedCurve()
	{
		return TEXT("FootPlanted");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FeetCrossingCurve()
	{
		return TEXT("FeetCrossing");
	}

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName PoseStandCurve()
	{
		return TEXT("PoseStand");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName PoseCrouchCurve()
	{
		return TEXT("PoseCrouch");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName PoseAirCurve()
	{
		return TEXT("PoseAir");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName GaitTypeCurve()
	{
		return TEXT("GaitType");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName RotationYawSpeedCurve()
	{
		return TEXT("RotationYawSpeed");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName RotationYawOffsetCurve()
	{
		return TEXT("RotationYawOffset");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName TransitionsAllowedCurve()
	{
		return TEXT("TransitionsAllowed");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName SprintBlockCurve()
	{
		return TEXT("SprintBlock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName GroundPredictionBlockCurve()
	{
		return TEXT("GroundPredictionBlock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootstepSoundBlockCurve()
	{
		return TEXT("FootstepSoundBlock");
	}
};

USTRUCT(BlueprintType)
struct ALS_API FAlsPropertyAccessConstants
{
	GENERATED_BODY()

	// Bones

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RootBone{UAlsConstants::RootBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PelvisBone{UAlsConstants::PelvisBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Spine03Bone{UAlsConstants::Spine03Bone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GunLeftVirtualBone{UAlsConstants::GunLeftVirtualBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GunRightVirtualBone{UAlsConstants::GunRightVirtualBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootLeftVirtualBone{UAlsConstants::FootLeftVirtualBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootRightVirtualBone{UAlsConstants::FootRightVirtualBone()};

	// Animation Slots

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TransitionSlot{UAlsConstants::TransitionSlot()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TurnInPlaceSlot{UAlsConstants::TurnInPlaceSlot()};

	// Layering Animation Curves

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerHeadCurve{UAlsConstants::LayerHeadCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerHeadAdditiveCurve{UAlsConstants::LayerHeadAdditiveCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerArmLeftCurve{UAlsConstants::LayerArmLeftCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerArmLeftAdditiveCurve{UAlsConstants::LayerArmLeftAdditiveCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerArmLeftLocalSpaceCurve{UAlsConstants::LayerArmLeftLocalSpaceCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerArmRightCurve{UAlsConstants::LayerArmRightCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerArmRightAdditiveCurve{UAlsConstants::LayerArmRightAdditiveCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerArmRightLocalSpaceCurve{UAlsConstants::LayerArmRightLocalSpaceCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerHandLeftCurve{UAlsConstants::LayerHandLeftCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerHandRightCurve{UAlsConstants::LayerHandRightCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerSpineCurve{UAlsConstants::LayerSpineCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerSpineAdditiveCurve{UAlsConstants::LayerSpineAdditiveCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerPelvisCurve{UAlsConstants::LayerPelvisCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LayerLegsCurve{UAlsConstants::LayerLegsCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HandLeftIkCurve{UAlsConstants::HandLeftIkCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HandRightIkCurve{UAlsConstants::HandRightIkCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AimManualCurve{UAlsConstants::AimManualCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AimBlockCurve{UAlsConstants::AimBlockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HipsDirectionLockCurve{UAlsConstants::HipsDirectionLockCurve()};

	// Feet Animation Curves

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootLeftIkCurve{UAlsConstants::FootLeftIkCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootLeftLockCurve{UAlsConstants::FootLeftLockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootRightIkCurve{UAlsConstants::FootRightIkCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootRightLockCurve{UAlsConstants::FootRightLockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootPlantedCurve{UAlsConstants::FootPlantedCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FeetCrossingCurve{UAlsConstants::FeetCrossingCurve()};

	// Other Animation Curves

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PoseStandCurve{UAlsConstants::PoseStandCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PoseCrouchCurve{UAlsConstants::PoseCrouchCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PoseAirCurve{UAlsConstants::PoseAirCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GaitTypeCurve{UAlsConstants::GaitTypeCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RotationYawSpeedCurve{UAlsConstants::RotationYawSpeedCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RotationYawOffsetCurve{UAlsConstants::RotationYawOffsetCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TransitionsAllowedCurve{UAlsConstants::TransitionsAllowedCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SprintBlockCurve{UAlsConstants::SprintBlockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GroundPredictionBlockCurve{UAlsConstants::GroundPredictionBlockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootstepSoundBlockCurve{UAlsConstants::FootstepSoundBlockCurve()};
};
