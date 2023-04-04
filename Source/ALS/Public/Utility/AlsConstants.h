#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALS_API UAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Bones

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& RootBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& PelvisBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& Spine03BoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandLeftGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandRightGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightVirtualBoneName();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TransitionSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceCrouchingSlotName();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& ViewBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowAimingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HipsDirectionLockCurveName();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseMovingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseStandingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseCrouchingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGroundedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseInAirCurveName();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootPlantedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FeetCrossingCurveName();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawSpeedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowTransitionsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& SprintBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& GroundPredictionBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootstepSoundBlockCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& StateDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& ShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& TracesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Als Constants", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& MantlingDebugDisplayName();
};

inline const FName& UAlsConstants::RootBoneName()
{
	static const FName Name{TEXTVIEW("root")};
	return Name;
}

inline const FName& UAlsConstants::PelvisBoneName()
{
	static const FName Name{TEXTVIEW("pelvis")};
	return Name;
}

inline const FName& UAlsConstants::HeadBoneName()
{
	static const FName Name{TEXTVIEW("head")};
	return Name;
}

inline const FName& UAlsConstants::Spine03BoneName()
{
	static const FName Name{TEXTVIEW("spine_03")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftBoneName()
{
	static const FName Name{TEXTVIEW("foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightBoneName()
{
	static const FName Name{TEXTVIEW("foot_r")};
	return Name;
}

inline const FName& UAlsConstants::HandLeftGunVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB hand_l_to_ik_hand_gun")};
	return Name;
}

inline const FName& UAlsConstants::HandRightGunVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB hand_r_to_ik_hand_gun")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftIkBoneName()
{
	static const FName Name{TEXTVIEW("ik_foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightIkBoneName()
{
	static const FName Name{TEXTVIEW("ik_foot_r")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB foot_l")};
	return Name;
}

inline const FName& UAlsConstants::FootRightVirtualBoneName()
{
	static const FName Name{TEXTVIEW("VB foot_r")};
	return Name;
}

inline const FName& UAlsConstants::TransitionSlotName()
{
	static const FName Name{TEXTVIEW("Transition")};
	return Name;
}

inline const FName& UAlsConstants::TurnInPlaceStandingSlotName()
{
	static const FName Name{TEXTVIEW("TurnInPlaceStanding")};
	return Name;
}

inline const FName& UAlsConstants::TurnInPlaceCrouchingSlotName()
{
	static const FName Name{TEXTVIEW("TurnInPlaceCrouching")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadCurveName()
{
	static const FName Name{TEXTVIEW("LayerHead")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerHeadAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerHeadSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerHeadSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeft")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftLocalSpaceCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftLocalSpace")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmLeftSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmLeftSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRight")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightLocalSpaceCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightLocalSpace")};
	return Name;
}

inline const FName& UAlsConstants::LayerArmRightSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerArmRightSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerHandLeftCurveName()
{
	static const FName Name{TEXTVIEW("LayerHandLeft")};
	return Name;
}

inline const FName& UAlsConstants::LayerHandRightCurveName()
{
	static const FName Name{TEXTVIEW("LayerHandRight")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpine")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineAdditiveCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpineAdditive")};
	return Name;
}

inline const FName& UAlsConstants::LayerSpineSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerSpineSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerPelvisCurveName()
{
	static const FName Name{TEXTVIEW("LayerPelvis")};
	return Name;
}

inline const FName& UAlsConstants::LayerPelvisSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerPelvisSlot")};
	return Name;
}

inline const FName& UAlsConstants::LayerLegsCurveName()
{
	static const FName Name{TEXTVIEW("LayerLegs")};
	return Name;
}

inline const FName& UAlsConstants::LayerLegsSlotCurveName()
{
	static const FName Name{TEXTVIEW("LayerLegsSlot")};
	return Name;
}

inline const FName& UAlsConstants::HandLeftIkCurveName()
{
	static const FName Name{TEXTVIEW("HandLeftIk")};
	return Name;
}

inline const FName& UAlsConstants::HandRightIkCurveName()
{
	static const FName Name{TEXTVIEW("HandRightIk")};
	return Name;
}

inline const FName& UAlsConstants::ViewBlockCurveName()
{
	static const FName Name{TEXTVIEW("ViewBlock")};
	return Name;
}

inline const FName& UAlsConstants::AllowAimingCurveName()
{
	static const FName Name{TEXTVIEW("AllowAiming")};
	return Name;
}

inline const FName& UAlsConstants::HipsDirectionLockCurveName()
{
	static const FName Name{TEXTVIEW("HipsDirectionLock")};
	return Name;
}

inline const FName& UAlsConstants::PoseGaitCurveName()
{
	static const FName Name{TEXTVIEW("PoseGait")};
	return Name;
}

inline const FName& UAlsConstants::PoseMovingCurveName()
{
	static const FName Name{TEXTVIEW("PoseMoving")};
	return Name;
}

inline const FName& UAlsConstants::PoseStandingCurveName()
{
	static const FName Name{TEXTVIEW("PoseStanding")};
	return Name;
}

inline const FName& UAlsConstants::PoseCrouchingCurveName()
{
	static const FName Name{TEXTVIEW("PoseCrouching")};
	return Name;
}

inline const FName& UAlsConstants::PoseGroundedCurveName()
{
	static const FName Name{TEXTVIEW("PoseGrounded")};
	return Name;
}

inline const FName& UAlsConstants::PoseInAirCurveName()
{
	static const FName Name{TEXTVIEW("PoseInAir")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftIkCurveName()
{
	static const FName Name{TEXTVIEW("FootLeftIk")};
	return Name;
}

inline const FName& UAlsConstants::FootLeftLockCurveName()
{
	static const FName Name{TEXTVIEW("FootLeftLock")};
	return Name;
}

inline const FName& UAlsConstants::FootRightIkCurveName()
{
	static const FName Name{TEXTVIEW("FootRightIk")};
	return Name;
}

inline const FName& UAlsConstants::FootRightLockCurveName()
{
	static const FName Name{TEXTVIEW("FootRightLock")};
	return Name;
}

inline const FName& UAlsConstants::FootPlantedCurveName()
{
	static const FName Name{TEXTVIEW("FootPlanted")};
	return Name;
}

inline const FName& UAlsConstants::FeetCrossingCurveName()
{
	static const FName Name{TEXTVIEW("FeetCrossing")};
	return Name;
}

inline const FName& UAlsConstants::RotationYawSpeedCurveName()
{
	static const FName Name{TEXTVIEW("RotationYawSpeed")};
	return Name;
}

inline const FName& UAlsConstants::RotationYawOffsetCurveName()
{
	static const FName Name{TEXTVIEW("RotationYawOffset")};
	return Name;
}

inline const FName& UAlsConstants::AllowTransitionsCurveName()
{
	static const FName Name{TEXTVIEW("AllowTransitions")};
	return Name;
}

inline const FName& UAlsConstants::SprintBlockCurveName()
{
	static const FName Name{TEXTVIEW("SprintBlock")};
	return Name;
}

inline const FName& UAlsConstants::GroundPredictionBlockCurveName()
{
	static const FName Name{TEXTVIEW("GroundPredictionBlock")};
	return Name;
}

inline const FName& UAlsConstants::FootstepSoundBlockCurveName()
{
	static const FName Name{TEXTVIEW("FootstepSoundBlock")};
	return Name;
}

inline const FName& UAlsConstants::CurvesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("ALS.Curves")};
	return Name;
}

inline const FName& UAlsConstants::StateDebugDisplayName()
{
	static const FName Name{TEXTVIEW("ALS.State")};
	return Name;
}

inline const FName& UAlsConstants::ShapesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("ALS.Shapes")};
	return Name;
}

inline const FName& UAlsConstants::TracesDebugDisplayName()
{
	static const FName Name{TEXTVIEW("ALS.Traces")};
	return Name;
}

inline const FName& UAlsConstants::MantlingDebugDisplayName()
{
	static const FName Name{TEXTVIEW("ALS.Mantling")};
	return Name;
}
