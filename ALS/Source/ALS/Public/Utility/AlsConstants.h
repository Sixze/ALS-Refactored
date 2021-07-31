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
	static const FName& RootBone()
	{
		static const FName Name{TEXT("root")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& PelvisBone()
	{
		static const FName Name{TEXT("pelvis")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HeadBone()
	{
		static const FName Name{TEXT("head")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& Spine03Bone()
	{
		static const FName Name{TEXT("spine_03")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftBone()
	{
		static const FName Name{TEXT("foot_l")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightBone()
	{
		static const FName Name{TEXT("foot_r")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HandLeftGunVirtualBone()
	{
		static const FName Name{TEXT("VB hand_l_to_ik_hand_gun")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HandRightGunVirtualBone()
	{
		static const FName Name{TEXT("VB hand_r_to_ik_hand_gun")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftIkBone()
	{
		static const FName Name{TEXT("ik_foot_l")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightIkBone()
	{
		static const FName Name{TEXT("ik_foot_r")};
		return Name;
	}

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TransitionSlot()
	{
		static const FName Name{TEXT("Transition")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TurnInPlaceSlot()
	{
		static const FName Name{TEXT("TurnInPlace")};
		return Name;
	}

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadCurve()
	{
		static const FName Name{TEXT("LayerHead")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadAdditiveCurve()
	{
		static const FName Name{TEXT("LayerHeadAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftCurve()
	{
		static const FName Name{TEXT("LayerArmLeft")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftAdditiveCurve()
	{
		static const FName Name{TEXT("LayerArmLeftAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftLocalSpaceCurve()
	{
		static const FName Name{TEXT("LayerArmLeftLocalSpace")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightCurve()
	{
		static const FName Name{TEXT("LayerArmRight")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightAdditiveCurve()
	{
		static const FName Name{TEXT("LayerArmRightAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightLocalSpaceCurve()
	{
		static const FName Name{TEXT("LayerArmRightLocalSpace")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHandLeftCurve()
	{
		static const FName Name{TEXT("LayerHandLeft")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHandRightCurve()
	{
		static const FName Name{TEXT("LayerHandRight")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineCurve()
	{
		static const FName Name{TEXT("LayerSpine")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineAdditiveCurve()
	{
		static const FName Name{TEXT("LayerSpineAdditive")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerPelvisCurve()
	{
		static const FName Name{TEXT("LayerPelvis")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerLegsCurve()
	{
		static const FName Name{TEXT("LayerLegs")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HandLeftIkCurve()
	{
		static const FName Name{TEXT("HandLeftIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HandRightIkCurve()
	{
		static const FName Name{TEXT("HandRightIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AimManualCurve()
	{
		static const FName Name{TEXT("AimManual")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AimBlockCurve()
	{
		static const FName Name{TEXT("AimBlock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HipsDirectionLockCurve()
	{
		static const FName Name{TEXT("HipsDirectionLock")};
		return Name;
	}

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootLeftIkCurve()
	{
		static const FName Name{TEXT("FootLeftIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootLeftLockCurve()
	{
		static const FName Name{TEXT("FootLeftLock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootRightIkCurve()
	{
		static const FName Name{TEXT("FootRightIk")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootRightLockCurve()
	{
		static const FName Name{TEXT("FootRightLock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootPlantedCurve()
	{
		static const FName Name{TEXT("FootPlanted")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FeetCrossingCurve()
	{
		static const FName Name{TEXT("FeetCrossing")};
		return Name;
	}

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseStandCurve()
	{
		static const FName Name{TEXT("PoseStand")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseCrouchCurve()
	{
		static const FName Name{TEXT("PoseCrouch")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseAirCurve()
	{
		static const FName Name{TEXT("PoseAir")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& GaitAmountCurve()
	{
		static const FName Name{TEXT("GaitAmount")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& RotationYawSpeedCurve()
	{
		static const FName Name{TEXT("RotationYawSpeed")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& RotationYawOffsetCurve()
	{
		static const FName Name{TEXT("RotationYawOffset")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AllowTransitionsCurve()
	{
		static const FName Name{TEXT("AllowTransitions")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& SprintBlockCurve()
	{
		static const FName Name{TEXT("SprintBlock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& GroundPredictionBlockCurve()
	{
		static const FName Name{TEXT("GroundPredictionBlock")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootstepSoundBlockCurve()
	{
		static const FName Name{TEXT("FootstepSoundBlock")};
		return Name;
	}

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& CurvesDisplayName()
	{
		static const FName Name{TEXT("ALS.Curves")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& StateDisplayName()
	{
		static const FName Name{TEXT("ALS.State")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& ShapesDisplayName()
	{
		static const FName Name{TEXT("ALS.Shapes")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& TracesDisplayName()
	{
		static const FName Name{TEXT("ALS.Traces")};
		return Name;
	}

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& MantlingDisplayName()
	{
		static const FName Name{TEXT("ALS.Mantling")};
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
	FName HandLeftGunVirtualBone{UAlsConstants::HandLeftGunVirtualBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HandRightGunVirtualBone{UAlsConstants::HandRightGunVirtualBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootLeftIkBone{UAlsConstants::FootLeftIkBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootRightIkBone{UAlsConstants::FootRightIkBone()};

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
	FName AllowTransitionsCurve{UAlsConstants::AllowTransitionsCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SprintBlockCurve{UAlsConstants::SprintBlockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GroundPredictionBlockCurve{UAlsConstants::GroundPredictionBlockCurve()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FootstepSoundBlockCurve{UAlsConstants::FootstepSoundBlockCurve()};
};
