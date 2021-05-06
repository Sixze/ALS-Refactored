#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "AlsConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALS_API UAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Bones

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName RootBone()
	{
		static const FName Name{TEXT("root")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName PelvisBone()
	{
		static const FName Name{TEXT("pelvis")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName Spine03Bone()
	{
		static const FName Name{TEXT("spine_03")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName GunLeftVirtualBone()
	{
		static const FName Name{TEXT("VB ik_hand_gun_l")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName GunRightVirtualBone()
	{
		static const FName Name{TEXT("VB ik_hand_gun_r")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName FootLeftVirtualBone()
	{
		static const FName Name{TEXT("VB ik_foot_l")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static FName FootRightVirtualBone()
	{
		static const FName Name{TEXT("VB ik_foot_r")};
		return Name;
	}

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static FName TransitionSlot()
	{
		static const FName Name{TEXT("Transition")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static FName TurnInPlaceSlot()
	{
		static const FName Name{TEXT("TurnInPlace")};
		return Name;
	}

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHeadCurve()
	{
		static const FName Name{TEXT("LayerHead")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHeadAdditiveCurve()
	{
		static const FName Name{TEXT("LayerHeadAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmLeftCurve()
	{
		static const FName Name{TEXT("LayerArmLeft")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmLeftAdditiveCurve()
	{
		static const FName Name{TEXT("LayerArmLeftAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmLeftLocalSpaceCurve()
	{
		static const FName Name{TEXT("LayerArmLeftLocalSpace")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmRightCurve()
	{
		static const FName Name{TEXT("LayerArmRight")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmRightAdditiveCurve()
	{
		static const FName Name{TEXT("LayerArmRightAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerArmRightLocalSpaceCurve()
	{
		static const FName Name{TEXT("LayerArmRightLocalSpace")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHandLeftCurve()
	{
		static const FName Name{TEXT("LayerHandLeft")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerHandRightCurve()
	{
		static const FName Name{TEXT("LayerHandRight")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerSpineCurve()
	{
		static const FName Name{TEXT("LayerSpine")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerSpineAdditiveCurve()
	{
		static const FName Name{TEXT("LayerSpineAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerPelvisCurve()
	{
		static const FName Name{TEXT("LayerPelvis")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName LayerLegsCurve()
	{
		static const FName Name{TEXT("LayerLegs")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName HandLeftIkCurve()
	{
		static const FName Name{TEXT("HandLeftIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName HandRightIkCurve()
	{
		static const FName Name{TEXT("HandRightIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName AimManualCurve()
	{
		static const FName Name{TEXT("AimManual")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName AimBlockCurve()
	{
		static const FName Name{TEXT("AimBlock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName HipsDirectionLockCurve()
	{
		static const FName Name{TEXT("HipsDirectionLock")};
		return Name;
	}

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootLeftIkCurve()
	{
		static const FName Name{TEXT("FootLeftIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootLeftLockCurve()
	{
		static const FName Name{TEXT("FootLeftLock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootRightIkCurve()
	{
		static const FName Name{TEXT("FootRightIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootRightLockCurve()
	{
		static const FName Name{TEXT("FootRightLock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootPlantedCurve()
	{
		static const FName Name{TEXT("FootPlanted")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FeetCrossingCurve()
	{
		static const FName Name{TEXT("FeetCrossing")};
		return Name;
	}

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName PoseStandCurve()
	{
		static const FName Name{TEXT("PoseStand")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName PoseCrouchCurve()
	{
		static const FName Name{TEXT("PoseCrouch")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName PoseAirCurve()
	{
		static const FName Name{TEXT("PoseAir")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName GaitAmountCurve()
	{
		static const FName Name{TEXT("GaitAmount")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName RotationYawSpeedCurve()
	{
		static const FName Name{TEXT("RotationYawSpeed")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName RotationYawOffsetCurve()
	{
		static const FName Name{TEXT("RotationYawOffset")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName TransitionsAllowedCurve()
	{
		static const FName Name{TEXT("TransitionsAllowed")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName SprintBlockCurve()
	{
		static const FName Name{TEXT("SprintBlock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName GroundPredictionBlockCurve()
	{
		static const FName Name{TEXT("GroundPredictionBlock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static FName FootstepSoundBlockCurve()
	{
		static const FName Name{TEXT("FootstepSoundBlock")};
		return Name;
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
	FName GaitAmountCurve{UAlsConstants::GaitAmountCurve()};

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
