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
	static const FName& RootBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& PelvisBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HeadBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& Spine03Bone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HandLeftGunVirtualBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HandRightGunVirtualBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftIkBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightIkBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftVirtualBone();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightVirtualBone();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TransitionSlot();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TurnInPlaceStandingSlot();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TurnInPlaceCrouchingSlot();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadAdditiveCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadSlotCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftAdditiveCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftLocalSpaceCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftSlotCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightAdditiveCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightLocalSpaceCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightSlotCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHandLeftCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHandRightCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineAdditiveCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineSlotCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerPelvisCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerPelvisSlotCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerLegsCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerLegsSlotCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HandLeftIkCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HandRightIkCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& ViewBlockCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AllowAimingCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HipsDirectionLockCurve();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseGaitCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseMovingCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseStandingCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseCrouchingCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseGroundedCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseInAirCurve();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootLeftIkCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootLeftLockCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootRightIkCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootRightLockCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootPlantedCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FeetCrossingCurve();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& RotationYawSpeedCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& RotationYawOffsetCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AllowTransitionsCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& SprintBlockCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& GroundPredictionBlockCurve();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootstepSoundBlockCurve();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& CurvesDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& StateDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& ShapesDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& TracesDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants")
	static const FName& MantlingDisplayName();
};

inline const FName& UAlsConstants::RootBone()
{
	static const FName Name{TEXT("root")};
	return Name;
}

inline const FName& UAlsConstants::PelvisBone()
{
	static const FName Name{TEXT("pelvis")};
	return Name;
}

inline const FName& UAlsConstants::HeadBone()
{
	static const FName Name{TEXT("head")};
	return Name;
}

inline const FName& UAlsConstants::Spine03Bone()
{
	static const FName Name{TEXT("spine_03")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftBone()
{
	static const FName Name{TEXT("foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightBone()
{
	static const FName Name{TEXT("foot_r")};
	return Name;
}

inline const FName& UAlsConstants::HandLeftGunVirtualBone()
{
	static const FName Name{TEXT("VB hand_l_to_ik_hand_gun")};
	return Name;
}

inline const FName& UAlsConstants::HandRightGunVirtualBone()
{
	static const FName Name{TEXT("VB hand_r_to_ik_hand_gun")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftIkBone()
{
	static const FName Name{TEXT("ik_foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightIkBone()
{
	static const FName Name{TEXT("ik_foot_r")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftVirtualBone()
{
	static const FName Name{TEXT("VB foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightVirtualBone()
{
	static const FName Name{TEXT("VB foot_r")};
	return Name;
}

inline const FName& UAlsConstants::TransitionSlot()
{
	static const FName Name{TEXT("Transition")};
	return Name;
}

inline const FName& UAlsConstants::TurnInPlaceStandingSlot()
{
	static const FName Name{TEXT("TurnInPlaceStanding")};
	return Name;
}

inline const FName& UAlsConstants::TurnInPlaceCrouchingSlot()
{
	static const FName Name{TEXT("TurnInPlaceCrouching")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadCurve()
{
	static const FName Name{TEXT("LayerHead")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadAdditiveCurve()
{
	static const FName Name{TEXT("LayerHeadAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadSlotCurve()
{
	static const FName Name{TEXT("LayerHeadSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftCurve()
{
	static const FName Name{TEXT("LayerArmLeft")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftAdditiveCurve()
{
	static const FName Name{TEXT("LayerArmLeftAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftLocalSpaceCurve()
{
	static const FName Name{TEXT("LayerArmLeftLocalSpace")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftSlotCurve()
{
	static const FName Name{TEXT("LayerArmLeftSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightCurve()
{
	static const FName Name{TEXT("LayerArmRight")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightAdditiveCurve()
{
	static const FName Name{TEXT("LayerArmRightAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightLocalSpaceCurve()
{
	static const FName Name{TEXT("LayerArmRightLocalSpace")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightSlotCurve()
{
	static const FName Name{TEXT("LayerArmRightSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerHandLeftCurve()
{
	static const FName Name{TEXT("LayerHandLeft")};
	return Name;
}

inline const FName& UAlsConstants::LayerHandRightCurve()
{
	static const FName Name{TEXT("LayerHandRight")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineCurve()
{
	static const FName Name{TEXT("LayerSpine")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineAdditiveCurve()
{
	static const FName Name{TEXT("LayerSpineAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineSlotCurve()
{
	static const FName Name{TEXT("LayerSpineSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerPelvisCurve()
{
	static const FName Name{TEXT("LayerPelvis")};
	return Name;
}

inline const FName& UAlsConstants::LayerPelvisSlotCurve()
{
	static const FName Name{TEXT("LayerPelvisSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerLegsCurve()
{
	static const FName Name{TEXT("LayerLegs")};
	return Name;
}

inline const FName& UAlsConstants::LayerLegsSlotCurve()
{
	static const FName Name{TEXT("LayerLegsSlot")};
	return Name;
}

inline const FName& UAlsConstants::HandLeftIkCurve()
{
	static const FName Name{TEXT("HandLeftIk")};
	return Name;
}

inline const FName& UAlsConstants::HandRightIkCurve()
{
	static const FName Name{TEXT("HandRightIk")};
	return Name;
}

inline const FName& UAlsConstants::ViewBlockCurve()
{
	static const FName Name{TEXT("ViewBlock")};
	return Name;
}

inline const FName& UAlsConstants::AllowAimingCurve()
{
	static const FName Name{TEXT("AllowAiming")};
	return Name;
}

inline const FName& UAlsConstants::HipsDirectionLockCurve()
{
	static const FName Name{TEXT("HipsDirectionLock")};
	return Name;
}

inline const FName& UAlsConstants::PoseGaitCurve()
{
	static const FName Name{TEXT("PoseGait")};
	return Name;
}

inline const FName& UAlsConstants::PoseMovingCurve()
{
	static const FName Name{TEXT("PoseMoving")};
	return Name;
}

inline const FName& UAlsConstants::PoseStandingCurve()
{
	static const FName Name{TEXT("PoseStanding")};
	return Name;
}

inline const FName& UAlsConstants::PoseCrouchingCurve()
{
	static const FName Name{TEXT("PoseCrouching")};
	return Name;
}

inline const FName& UAlsConstants::PoseGroundedCurve()
{
	static const FName Name{TEXT("PoseGrounded")};
	return Name;
}

inline const FName& UAlsConstants::PoseInAirCurve()
{
	static const FName Name{TEXT("PoseInAir")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftIkCurve()
{
	static const FName Name{TEXT("FootLeftIk")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftLockCurve()
{
	static const FName Name{TEXT("FootLeftLock")};
	return Name;
}

inline const FName& UAlsConstants::FootRightIkCurve()
{
	static const FName Name{TEXT("FootRightIk")};
	return Name;
}

inline const FName& UAlsConstants::FootRightLockCurve()
{
	static const FName Name{TEXT("FootRightLock")};
	return Name;
}

inline const FName& UAlsConstants::FootPlantedCurve()
{
	static const FName Name{TEXT("FootPlanted")};
	return Name;
}

inline const FName& UAlsConstants::FeetCrossingCurve()
{
	static const FName Name{TEXT("FeetCrossing")};
	return Name;
}

inline const FName& UAlsConstants::RotationYawSpeedCurve()
{
	static const FName Name{TEXT("RotationYawSpeed")};
	return Name;
}

inline const FName& UAlsConstants::RotationYawOffsetCurve()
{
	static const FName Name{TEXT("RotationYawOffset")};
	return Name;
}

inline const FName& UAlsConstants::AllowTransitionsCurve()
{
	static const FName Name{TEXT("AllowTransitions")};
	return Name;
}

inline const FName& UAlsConstants::SprintBlockCurve()
{
	static const FName Name{TEXT("SprintBlock")};
	return Name;
}

inline const FName& UAlsConstants::GroundPredictionBlockCurve()
{
	static const FName Name{TEXT("GroundPredictionBlock")};
	return Name;
}

inline const FName& UAlsConstants::FootstepSoundBlockCurve()
{
	static const FName Name{TEXT("FootstepSoundBlock")};
	return Name;
}

inline const FName& UAlsConstants::CurvesDisplayName()
{
	static const FName Name{TEXT("ALS.Curves")};
	return Name;
}

inline const FName& UAlsConstants::StateDisplayName()
{
	static const FName Name{TEXT("ALS.State")};
	return Name;
}

inline const FName& UAlsConstants::ShapesDisplayName()
{
	static const FName Name{TEXT("ALS.Shapes")};
	return Name;
}

inline const FName& UAlsConstants::TracesDisplayName()
{
	static const FName Name{TEXT("ALS.Traces")};
	return Name;
}

inline const FName& UAlsConstants::MantlingDisplayName()
{
	static const FName Name{TEXT("ALS.Mantling")};
	return Name;
}
