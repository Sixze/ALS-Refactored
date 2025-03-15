#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALS_API UAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	// Bones

	inline static const FName RootBone{TEXTVIEW("root")};
	inline static const FName PelvisBone{TEXTVIEW("pelvis")};
	inline static const FName HeadBone{TEXTVIEW("head")};
	inline static const FName Spine03Bone{TEXTVIEW("spine_03")};
	inline static const FName FootLeftBone{TEXTVIEW("foot_l")};
	inline static const FName FootRightBone{TEXTVIEW("foot_r")};
	inline static const FName HandLeftGunVirtualBone{TEXTVIEW("VB hand_l_to_ik_hand_gun")};
	inline static const FName HandRightGunVirtualBone{TEXTVIEW("VB hand_r_to_ik_hand_gun")};
	inline static const FName FootLeftIkBone{TEXTVIEW("ik_foot_l")};
	inline static const FName FootRightIkBone{TEXTVIEW("ik_foot_r")};
	inline static const FName FootLeftVirtualBone{TEXTVIEW("VB foot_l")};

	// Animation Slots

	inline static const FName FootRightVirtualBone{TEXTVIEW("VB foot_r")};
	inline static const FName TransitionSlot{TEXTVIEW("Transition")};
	inline static const FName TurnInPlaceStandingSlot{TEXTVIEW("TurnInPlaceStanding")};

	// Layering Animation Curves

	inline static const FName TurnInPlaceCrouchingSlot{TEXTVIEW("TurnInPlaceCrouching")};
	inline static const FName LayerHeadCurve{TEXTVIEW("LayerHead")};
	inline static const FName LayerHeadAdditiveCurve{TEXTVIEW("LayerHeadAdditive")};
	inline static const FName LayerHeadSlotCurve{TEXTVIEW("LayerHeadSlot")};
	inline static const FName LayerArmLeftCurve{TEXTVIEW("LayerArmLeft")};
	inline static const FName LayerArmLeftAdditiveCurve{TEXTVIEW("LayerArmLeftAdditive")};
	inline static const FName LayerArmLeftLocalSpaceCurve{TEXTVIEW("LayerArmLeftLocalSpace")};
	inline static const FName LayerArmLeftSlotCurve{TEXTVIEW("LayerArmLeftSlot")};
	inline static const FName LayerArmRightCurve{TEXTVIEW("LayerArmRight")};
	inline static const FName LayerArmRightAdditiveCurve{TEXTVIEW("LayerArmRightAdditive")};
	inline static const FName LayerArmRightLocalSpaceCurve{TEXTVIEW("LayerArmRightLocalSpace")};
	inline static const FName LayerArmRightSlotCurve{TEXTVIEW("LayerArmRightSlot")};
	inline static const FName LayerHandLeftCurve{TEXTVIEW("LayerHandLeft")};
	inline static const FName LayerHandRightCurve{TEXTVIEW("LayerHandRight")};
	inline static const FName LayerSpineCurve{TEXTVIEW("LayerSpine")};
	inline static const FName LayerSpineAdditiveCurve{TEXTVIEW("LayerSpineAdditive")};
	inline static const FName LayerSpineSlotCurve{TEXTVIEW("LayerSpineSlot")};
	inline static const FName LayerPelvisCurve{TEXTVIEW("LayerPelvis")};
	inline static const FName LayerPelvisSlotCurve{TEXTVIEW("LayerPelvisSlot")};
	inline static const FName LayerLegsCurve{TEXTVIEW("LayerLegs")};
	inline static const FName LayerLegsSlotCurve{TEXTVIEW("LayerLegsSlot")};
	inline static const FName HandLeftIkCurve{TEXTVIEW("HandLeftIk")};
	inline static const FName HandRightIkCurve{TEXTVIEW("HandRightIk")};
	inline static const FName ViewBlockCurve{TEXTVIEW("ViewBlock")};
	inline static const FName AllowAimingCurve{TEXTVIEW("AllowAiming")};
	inline static const FName HipsDirectionLockCurve{TEXTVIEW("HipsDirectionLock")};

	// Pose Animation Curves

	inline static const FName PoseGaitCurve{TEXTVIEW("PoseGait")};
	inline static const FName PoseMovingCurve{TEXTVIEW("PoseMoving")};
	inline static const FName PoseStandingCurve{TEXTVIEW("PoseStanding")};
	inline static const FName PoseCrouchingCurve{TEXTVIEW("PoseCrouching")};
	inline static const FName PoseGroundedCurve{TEXTVIEW("PoseGrounded")};
	inline static const FName PoseInAirCurve{TEXTVIEW("PoseInAir")};

	// Feet Animation Curves

	inline static const FName FootLeftIkCurve{TEXTVIEW("FootLeftIk")};
	inline static const FName FootLeftLockCurve{TEXTVIEW("FootLeftLock")};
	inline static const FName FootRightIkCurve{TEXTVIEW("FootRightIk")};
	inline static const FName FootRightLockCurve{TEXTVIEW("FootRightLock")};
	inline static const FName FootPlantedCurve{TEXTVIEW("FootPlanted")};
	inline static const FName FeetCrossingCurve{TEXTVIEW("FeetCrossing")};

	// Other Animation Curves

	inline static const FName RotationYawSpeedCurve{TEXTVIEW("RotationYawSpeed")};
	inline static const FName RotationYawOffsetCurve{TEXTVIEW("RotationYawOffset")};
	inline static const FName AllowTransitionsCurve{TEXTVIEW("AllowTransitions")};
	inline static const FName SprintBlockCurve{TEXTVIEW("SprintBlock")};
	inline static const FName GroundPredictionBlockCurve{TEXTVIEW("GroundPredictionBlock")};
	inline static const FName FootstepSoundBlockCurve{TEXTVIEW("FootstepSoundBlock")};

	// Debug

	inline static const FName CurvesDebugDisplay{TEXTVIEW("ALS.Curves")};
	inline static const FName StateDebugDisplay{TEXTVIEW("ALS.State")};
	inline static const FName ShapesDebugDisplay{TEXTVIEW("ALS.Shapes")};
	inline static const FName TracesDebugDisplay{TEXTVIEW("ALS.Traces")};
	inline static const FName MantlingDebugDisplay{TEXTVIEW("ALS.Mantling")};

public:
	// Bones

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& RootBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& PelvisBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& Spine03BoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandLeftGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& HandRightGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootLeftVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static const FName& FootRightVirtualBoneName();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TransitionSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static const FName& TurnInPlaceCrouchingSlotName();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHeadSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmLeftSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerArmRightSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerHandRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerSpineSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerPelvisSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& LayerLegsSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HandRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& ViewBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowAimingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& HipsDirectionLockCurveName();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseMovingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseStandingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseCrouchingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseGroundedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& PoseInAirCurveName();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootLeftLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootRightLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootPlantedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FeetCrossingCurveName();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawSpeedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& RotationYawOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& AllowTransitionsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& SprintBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& GroundPredictionBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static const FName& FootstepSoundBlockCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& CurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& StateDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& ShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& TracesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static const FName& MantlingDebugDisplayName();
};

inline const FName& UAlsConstants::RootBoneName()
{
	return RootBone;
}

inline const FName& UAlsConstants::PelvisBoneName()
{
	return PelvisBone;
}

inline const FName& UAlsConstants::HeadBoneName()
{
	return HeadBone;
}

inline const FName& UAlsConstants::Spine03BoneName()
{
	return Spine03Bone;
}

inline const FName& UAlsConstants::FootLeftBoneName()
{
	return FootLeftBone;
}

inline const FName& UAlsConstants::FootRightBoneName()
{
	return FootRightBone;
}

inline const FName& UAlsConstants::HandLeftGunVirtualBoneName()
{
	return HandLeftGunVirtualBone;
}

inline const FName& UAlsConstants::HandRightGunVirtualBoneName()
{
	return HandRightGunVirtualBone;
}

inline const FName& UAlsConstants::FootLeftIkBoneName()
{
	return FootLeftIkBone;
}

inline const FName& UAlsConstants::FootRightIkBoneName()
{
	return FootRightIkBone;
}

inline const FName& UAlsConstants::FootLeftVirtualBoneName()
{
	return FootLeftVirtualBone;
}

inline const FName& UAlsConstants::FootRightVirtualBoneName()
{
	return FootRightVirtualBone;
}

inline const FName& UAlsConstants::TransitionSlotName()
{
	return TransitionSlot;
}

inline const FName& UAlsConstants::TurnInPlaceStandingSlotName()
{
	return TurnInPlaceStandingSlot;
}

inline const FName& UAlsConstants::TurnInPlaceCrouchingSlotName()
{
	return TurnInPlaceCrouchingSlot;
}

inline const FName& UAlsConstants::LayerHeadCurveName()
{
	return LayerHeadCurve;
}

inline const FName& UAlsConstants::LayerHeadAdditiveCurveName()
{
	return LayerHeadAdditiveCurve;
}

inline const FName& UAlsConstants::LayerHeadSlotCurveName()
{
	return LayerHeadSlotCurve;
}

inline const FName& UAlsConstants::LayerArmLeftCurveName()
{
	return LayerArmLeftCurve;
}

inline const FName& UAlsConstants::LayerArmLeftAdditiveCurveName()
{
	return LayerArmLeftAdditiveCurve;
}

inline const FName& UAlsConstants::LayerArmLeftLocalSpaceCurveName()
{
	return LayerArmLeftLocalSpaceCurve;
}

inline const FName& UAlsConstants::LayerArmLeftSlotCurveName()
{
	return LayerArmLeftSlotCurve;
}

inline const FName& UAlsConstants::LayerArmRightCurveName()
{
	return LayerArmRightCurve;
}

inline const FName& UAlsConstants::LayerArmRightAdditiveCurveName()
{
	return LayerArmRightAdditiveCurve;
}

inline const FName& UAlsConstants::LayerArmRightLocalSpaceCurveName()
{
	return LayerArmRightLocalSpaceCurve;
}

inline const FName& UAlsConstants::LayerArmRightSlotCurveName()
{
	return LayerArmRightSlotCurve;
}

inline const FName& UAlsConstants::LayerHandLeftCurveName()
{
	return LayerHandLeftCurve;
}

inline const FName& UAlsConstants::LayerHandRightCurveName()
{
	return LayerHandRightCurve;
}

inline const FName& UAlsConstants::LayerSpineCurveName()
{
	return LayerSpineCurve;
}

inline const FName& UAlsConstants::LayerSpineAdditiveCurveName()
{
	return LayerSpineAdditiveCurve;
}

inline const FName& UAlsConstants::LayerSpineSlotCurveName()
{
	return LayerSpineSlotCurve;
}

inline const FName& UAlsConstants::LayerPelvisCurveName()
{
	return LayerPelvisCurve;
}

inline const FName& UAlsConstants::LayerPelvisSlotCurveName()
{
	return LayerPelvisSlotCurve;
}

inline const FName& UAlsConstants::LayerLegsCurveName()
{
	return LayerLegsCurve;
}

inline const FName& UAlsConstants::LayerLegsSlotCurveName()
{
	return LayerLegsSlotCurve;
}

inline const FName& UAlsConstants::HandLeftIkCurveName()
{
	return HandLeftIkCurve;
}

inline const FName& UAlsConstants::HandRightIkCurveName()
{
	return HandRightIkCurve;
}

inline const FName& UAlsConstants::ViewBlockCurveName()
{
	return ViewBlockCurve;
}

inline const FName& UAlsConstants::AllowAimingCurveName()
{
	return AllowAimingCurve;
}

inline const FName& UAlsConstants::HipsDirectionLockCurveName()
{
	return HipsDirectionLockCurve;
}

inline const FName& UAlsConstants::PoseGaitCurveName()
{
	return PoseGaitCurve;
}

inline const FName& UAlsConstants::PoseMovingCurveName()
{
	return PoseMovingCurve;
}

inline const FName& UAlsConstants::PoseStandingCurveName()
{
	return PoseStandingCurve;
}

inline const FName& UAlsConstants::PoseCrouchingCurveName()
{
	return PoseCrouchingCurve;
}

inline const FName& UAlsConstants::PoseGroundedCurveName()
{
	return PoseGroundedCurve;
}

inline const FName& UAlsConstants::PoseInAirCurveName()
{
	return PoseInAirCurve;
}

inline const FName& UAlsConstants::FootLeftIkCurveName()
{
	return FootLeftIkCurve;
}

inline const FName& UAlsConstants::FootLeftLockCurveName()
{
	return FootLeftLockCurve;
}

inline const FName& UAlsConstants::FootRightIkCurveName()
{
	return FootRightIkCurve;
}

inline const FName& UAlsConstants::FootRightLockCurveName()
{
	return FootRightLockCurve;
}

inline const FName& UAlsConstants::FootPlantedCurveName()
{
	return FootPlantedCurve;
}

inline const FName& UAlsConstants::FeetCrossingCurveName()
{
	return FeetCrossingCurve;
}

inline const FName& UAlsConstants::RotationYawSpeedCurveName()
{
	return RotationYawSpeedCurve;
}

inline const FName& UAlsConstants::RotationYawOffsetCurveName()
{
	return RotationYawOffsetCurve;
}

inline const FName& UAlsConstants::AllowTransitionsCurveName()
{
	return AllowTransitionsCurve;
}

inline const FName& UAlsConstants::SprintBlockCurveName()
{
	return SprintBlockCurve;
}

inline const FName& UAlsConstants::GroundPredictionBlockCurveName()
{
	return GroundPredictionBlockCurve;
}

inline const FName& UAlsConstants::FootstepSoundBlockCurveName()
{
	return FootstepSoundBlockCurve;
}

inline const FName& UAlsConstants::CurvesDebugDisplayName()
{
	return CurvesDebugDisplay;
}

inline const FName& UAlsConstants::StateDebugDisplayName()
{
	return StateDebugDisplay;
}

inline const FName& UAlsConstants::ShapesDebugDisplayName()
{
	return ShapesDebugDisplay;
}

inline const FName& UAlsConstants::TracesDebugDisplayName()
{
	return TracesDebugDisplay;
}

inline const FName& UAlsConstants::MantlingDebugDisplayName()
{
	return MantlingDebugDisplay;
}
