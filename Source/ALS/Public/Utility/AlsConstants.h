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
	static const FName& RootBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& PelvisBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& Spine03BoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HandLeftGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& HandRightGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootLeftVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones")
	static const FName& FootRightVirtualBoneName();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TransitionSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots")
	static const FName& TurnInPlaceCrouchingSlotName();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHeadSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmLeftSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerArmRightSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHandLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerHandRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerSpineSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerPelvisCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerPelvisSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerLegsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& LayerLegsSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HandLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HandRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& ViewBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AllowAimingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& HipsDirectionLockCurveName();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseMovingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseStandingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseCrouchingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseGroundedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& PoseInAirCurveName();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootLeftLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootRightLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootPlantedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FeetCrossingCurveName();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& RotationYawSpeedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& RotationYawOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& AllowTransitionsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& SprintBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& GroundPredictionBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves")
	static const FName& FootstepSoundBlockCurveName();

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

inline const FName& UAlsConstants::RootBoneName()
{
	static const FName Name{TEXT("root")};
	return Name;
}

inline const FName& UAlsConstants::PelvisBoneName()
{
	static const FName Name{TEXT("pelvis")};
	return Name;
}

inline const FName& UAlsConstants::HeadBoneName()
{
	static const FName Name{TEXT("head")};
	return Name;
}

inline const FName& UAlsConstants::Spine03BoneName()
{
	static const FName Name{TEXT("spine_03")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftBoneName()
{
	static const FName Name{TEXT("foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightBoneName()
{
	static const FName Name{TEXT("foot_r")};
	return Name;
}

inline const FName& UAlsConstants::HandLeftGunVirtualBoneName()
{
	static const FName Name{TEXT("VB hand_l_to_ik_hand_gun")};
	return Name;
}

inline const FName& UAlsConstants::HandRightGunVirtualBoneName()
{
	static const FName Name{TEXT("VB hand_r_to_ik_hand_gun")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftIkBoneName()
{
	static const FName Name{TEXT("ik_foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightIkBoneName()
{
	static const FName Name{TEXT("ik_foot_r")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftVirtualBoneName()
{
	static const FName Name{TEXT("VB foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightVirtualBoneName()
{
	static const FName Name{TEXT("VB foot_r")};
	return Name;
}

inline const FName& UAlsConstants::TransitionSlotName()
{
	static const FName Name{TEXT("Transition")};
	return Name;
}

inline const FName& UAlsConstants::TurnInPlaceStandingSlotName()
{
	static const FName Name{TEXT("TurnInPlaceStanding")};
	return Name;
}

inline const FName& UAlsConstants::TurnInPlaceCrouchingSlotName()
{
	static const FName Name{TEXT("TurnInPlaceCrouching")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadCurveName()
{
	static const FName Name{TEXT("LayerHead")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadAdditiveCurveName()
{
	static const FName Name{TEXT("LayerHeadAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadSlotCurveName()
{
	static const FName Name{TEXT("LayerHeadSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftCurveName()
{
	static const FName Name{TEXT("LayerArmLeft")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftAdditiveCurveName()
{
	static const FName Name{TEXT("LayerArmLeftAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftLocalSpaceCurveName()
{
	static const FName Name{TEXT("LayerArmLeftLocalSpace")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftSlotCurveName()
{
	static const FName Name{TEXT("LayerArmLeftSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightCurveName()
{
	static const FName Name{TEXT("LayerArmRight")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightAdditiveCurveName()
{
	static const FName Name{TEXT("LayerArmRightAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightLocalSpaceCurveName()
{
	static const FName Name{TEXT("LayerArmRightLocalSpace")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightSlotCurveName()
{
	static const FName Name{TEXT("LayerArmRightSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerHandLeftCurveName()
{
	static const FName Name{TEXT("LayerHandLeft")};
	return Name;
}

inline const FName& UAlsConstants::LayerHandRightCurveName()
{
	static const FName Name{TEXT("LayerHandRight")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineCurveName()
{
	static const FName Name{TEXT("LayerSpine")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineAdditiveCurveName()
{
	static const FName Name{TEXT("LayerSpineAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineSlotCurveName()
{
	static const FName Name{TEXT("LayerSpineSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerPelvisCurveName()
{
	static const FName Name{TEXT("LayerPelvis")};
	return Name;
}

inline const FName& UAlsConstants::LayerPelvisSlotCurveName()
{
	static const FName Name{TEXT("LayerPelvisSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerLegsCurveName()
{
	static const FName Name{TEXT("LayerLegs")};
	return Name;
}

inline const FName& UAlsConstants::LayerLegsSlotCurveName()
{
	static const FName Name{TEXT("LayerLegsSlot")};
	return Name;
}

inline const FName& UAlsConstants::HandLeftIkCurveName()
{
	static const FName Name{TEXT("HandLeftIk")};
	return Name;
}

inline const FName& UAlsConstants::HandRightIkCurveName()
{
	static const FName Name{TEXT("HandRightIk")};
	return Name;
}

inline const FName& UAlsConstants::ViewBlockCurveName()
{
	static const FName Name{TEXT("ViewBlock")};
	return Name;
}

inline const FName& UAlsConstants::AllowAimingCurveName()
{
	static const FName Name{TEXT("AllowAiming")};
	return Name;
}

inline const FName& UAlsConstants::HipsDirectionLockCurveName()
{
	static const FName Name{TEXT("HipsDirectionLock")};
	return Name;
}

inline const FName& UAlsConstants::PoseGaitCurveName()
{
	static const FName Name{TEXT("PoseGait")};
	return Name;
}

inline const FName& UAlsConstants::PoseMovingCurveName()
{
	static const FName Name{TEXT("PoseMoving")};
	return Name;
}

inline const FName& UAlsConstants::PoseStandingCurveName()
{
	static const FName Name{TEXT("PoseStanding")};
	return Name;
}

inline const FName& UAlsConstants::PoseCrouchingCurveName()
{
	static const FName Name{TEXT("PoseCrouching")};
	return Name;
}

inline const FName& UAlsConstants::PoseGroundedCurveName()
{
	static const FName Name{TEXT("PoseGrounded")};
	return Name;
}

inline const FName& UAlsConstants::PoseInAirCurveName()
{
	static const FName Name{TEXT("PoseInAir")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftIkCurveName()
{
	static const FName Name{TEXT("FootLeftIk")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftLockCurveName()
{
	static const FName Name{TEXT("FootLeftLock")};
	return Name;
}

inline const FName& UAlsConstants::FootRightIkCurveName()
{
	static const FName Name{TEXT("FootRightIk")};
	return Name;
}

inline const FName& UAlsConstants::FootRightLockCurveName()
{
	static const FName Name{TEXT("FootRightLock")};
	return Name;
}

inline const FName& UAlsConstants::FootPlantedCurveName()
{
	static const FName Name{TEXT("FootPlanted")};
	return Name;
}

inline const FName& UAlsConstants::FeetCrossingCurveName()
{
	static const FName Name{TEXT("FeetCrossing")};
	return Name;
}

inline const FName& UAlsConstants::RotationYawSpeedCurveName()
{
	static const FName Name{TEXT("RotationYawSpeed")};
	return Name;
}

inline const FName& UAlsConstants::RotationYawOffsetCurveName()
{
	static const FName Name{TEXT("RotationYawOffset")};
	return Name;
}

inline const FName& UAlsConstants::AllowTransitionsCurveName()
{
	static const FName Name{TEXT("AllowTransitions")};
	return Name;
}

inline const FName& UAlsConstants::SprintBlockCurveName()
{
	static const FName Name{TEXT("SprintBlock")};
	return Name;
}

inline const FName& UAlsConstants::GroundPredictionBlockCurveName()
{
	static const FName Name{TEXT("GroundPredictionBlock")};
	return Name;
}

inline const FName& UAlsConstants::FootstepSoundBlockCurveName()
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
