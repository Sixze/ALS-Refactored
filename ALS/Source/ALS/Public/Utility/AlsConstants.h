#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "AlsConstants.generated.h"

UCLASS()
class ALS_API UAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Other", meta = (BlueprintThreadSafe))
	static FName FinalRagdollPose()
	{
		return TEXT("FinalRagdollPose");
	}

	// Bones

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName RootBone()
	{
		return TEXT("root");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName PelvisBone()
	{
		return TEXT("pelvis");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName Spine03Bone()
	{
		return TEXT("spine_03");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName GunLeftVirtualBone()
	{
		return TEXT("VB ik_hand_gun_l");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName GunRightVirtualBone()
	{
		return TEXT("VB ik_hand_gun_r");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName FootLeftLockVirtualBone()
	{
		return TEXT("VB ik_foot_l_lock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName FootRightLockVirtualBone()
	{
		return TEXT("VB ik_foot_r_lock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName FootLeftVirtualBone()
	{
		return TEXT("VB foot_l");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", meta = (BlueprintThreadSafe))
	static FName FootRightVirtualBone()
	{
		return TEXT("VB foot_r");
	}

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots", meta = (BlueprintThreadSafe))
	static FName TransitionSlot()
	{
		return TEXT("Transition");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots", meta = (BlueprintThreadSafe))
	static FName TurnInPlaceSlot()
	{
		return TEXT("TurnInPlace");
	}

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerHeadCurve()
	{
		return TEXT("LayerHead");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerHeadAdditiveCurve()
	{
		return TEXT("LayerHeadAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerArmLeftCurve()
	{
		return TEXT("LayerArmLeft");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerArmLeftAdditiveCurve()
	{
		return TEXT("LayerArmLeftAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerArmLeftLocalSpaceCurve()
	{
		return TEXT("LayerArmLeftLocalSpace");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerArmRightCurve()
	{
		return TEXT("LayerArmRight");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerArmRightAdditiveCurve()
	{
		return TEXT("LayerArmRightAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerArmRightLocalSpaceCurve()
	{
		return TEXT("LayerArmRightLocalSpace");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerHandLeftCurve()
	{
		return TEXT("LayerHandLeft");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerHandRightCurve()
	{
		return TEXT("LayerHandRight");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerSpineCurve()
	{
		return TEXT("LayerSpine");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerSpineAdditiveCurve()
	{
		return TEXT("LayerSpineAdditive");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerPelvisCurve()
	{
		return TEXT("LayerPelvis");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName LayerLegsCurve()
	{
		return TEXT("LayerLegs");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName HandLeftIkCurve()
	{
		return TEXT("HandLeftIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName HandRightIkCurve()
	{
		return TEXT("HandRightIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName AimManualCurve()
	{
		return TEXT("AimManual");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName AimBlockCurve()
	{
		return TEXT("AimBlock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName HipsDirectionLockCurve()
	{
		return TEXT("HipsDirectionLock");
	}

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FootLeftIkCurve()
	{
		return TEXT("FootLeftIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FootLeftLockCurve()
	{
		return TEXT("FootLeftLock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FootRightIkCurve()
	{
		return TEXT("FootRightIk");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FootRightLockCurve()
	{
		return TEXT("FootRightLock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FootPlantedCurve()
	{
		return TEXT("FootPlanted");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FeetCrossingCurve()
	{
		return TEXT("FeetCrossing");
	}

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName PoseStandCurve()
	{
		return TEXT("PoseStand");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName PoseCrouchCurve()
	{
		return TEXT("PoseCrouch");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName PoseAirCurve()
	{
		return TEXT("PoseAir");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName GaitTypeCurve()
	{
		return TEXT("GaitType");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName RotationYawSpeedCurve()
	{
		return TEXT("RotationYawSpeed");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName RotationYawOffsetCurve()
	{
		return TEXT("RotationYawOffset");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName TransitionsAllowedCurve()
	{
		return TEXT("TransitionsAllowed");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName SprintBlockCurve()
	{
		return TEXT("SprintBlock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName GroundPredictionBlockCurve()
	{
		return TEXT("GroundPredictionBlock");
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", meta = (BlueprintThreadSafe))
	static FName FootstepSoundBlockCurve()
	{
		return TEXT("FootstepSoundBlock");
	}
};

USTRUCT(BlueprintType)
struct ALS_API FAlsPropertyAccessConstants
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FinalRagdollPose{UAlsConstants::FinalRagdollPose()};

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
	FName FootLeftLockVirtualBone{UAlsConstants::FootLeftLockVirtualBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootRightLockVirtualBone{UAlsConstants::FootRightLockVirtualBone()};

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
