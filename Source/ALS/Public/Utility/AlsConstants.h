#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AlsConstants.generated.h"

UCLASS(Meta = (BlueprintThreadSafe))
class ALS_API UAlsConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	// Bones

	inline static FName RootBone{TEXTVIEW("root")};
	inline static FName PelvisBone{TEXTVIEW("pelvis")};
	inline static FName HeadBone{TEXTVIEW("head")};
	inline static FName Spine03Bone{TEXTVIEW("spine_03")};
	inline static FName FootLeftBone{TEXTVIEW("foot_l")};
	inline static FName FootRightBone{TEXTVIEW("foot_r")};
	inline static FName HandLeftGunVirtualBone{TEXTVIEW("VB hand_l_to_ik_hand_gun")};
	inline static FName HandRightGunVirtualBone{TEXTVIEW("VB hand_r_to_ik_hand_gun")};
	inline static FName FootLeftIkBone{TEXTVIEW("ik_foot_l")};
	inline static FName FootRightIkBone{TEXTVIEW("ik_foot_r")};
	inline static FName FootLeftVirtualBone{TEXTVIEW("VB foot_l")};
	inline static FName FootRightVirtualBone{TEXTVIEW("VB foot_r")};

	// Animation Slots

	inline static FName TransitionSlot{TEXTVIEW("Transition")};
	inline static FName TurnInPlaceStandingSlot{TEXTVIEW("TurnInPlaceStanding")};
	inline static FName TurnInPlaceCrouchingSlot{TEXTVIEW("TurnInPlaceCrouching")};

	// Layering Animation Curves

	inline static FName LayerHeadCurve{TEXTVIEW("LayerHead")};
	inline static FName LayerHeadAdditiveCurve{TEXTVIEW("LayerHeadAdditive")};
	inline static FName LayerHeadSlotCurve{TEXTVIEW("LayerHeadSlot")};
	inline static FName LayerArmLeftCurve{TEXTVIEW("LayerArmLeft")};
	inline static FName LayerArmLeftAdditiveCurve{TEXTVIEW("LayerArmLeftAdditive")};
	inline static FName LayerArmLeftLocalSpaceCurve{TEXTVIEW("LayerArmLeftLocalSpace")};
	inline static FName LayerArmLeftSlotCurve{TEXTVIEW("LayerArmLeftSlot")};
	inline static FName LayerArmRightCurve{TEXTVIEW("LayerArmRight")};
	inline static FName LayerArmRightAdditiveCurve{TEXTVIEW("LayerArmRightAdditive")};
	inline static FName LayerArmRightLocalSpaceCurve{TEXTVIEW("LayerArmRightLocalSpace")};
	inline static FName LayerArmRightSlotCurve{TEXTVIEW("LayerArmRightSlot")};
	inline static FName LayerHandLeftCurve{TEXTVIEW("LayerHandLeft")};
	inline static FName LayerHandRightCurve{TEXTVIEW("LayerHandRight")};
	inline static FName LayerSpineCurve{TEXTVIEW("LayerSpine")};
	inline static FName LayerSpineAdditiveCurve{TEXTVIEW("LayerSpineAdditive")};
	inline static FName LayerSpineSlotCurve{TEXTVIEW("LayerSpineSlot")};
	inline static FName LayerPelvisCurve{TEXTVIEW("LayerPelvis")};
	inline static FName LayerPelvisSlotCurve{TEXTVIEW("LayerPelvisSlot")};
	inline static FName LayerLegsCurve{TEXTVIEW("LayerLegs")};
	inline static FName LayerLegsSlotCurve{TEXTVIEW("LayerLegsSlot")};
	inline static FName HandLeftIkCurve{TEXTVIEW("HandLeftIk")};
	inline static FName HandRightIkCurve{TEXTVIEW("HandRightIk")};
	inline static FName ViewBlockCurve{TEXTVIEW("ViewBlock")};
	inline static FName AllowAimingCurve{TEXTVIEW("AllowAiming")};
	inline static FName HipsDirectionLockCurve{TEXTVIEW("HipsDirectionLock")};

	// Pose Animation Curves

	inline static FName PoseGaitCurve{TEXTVIEW("PoseGait")};
	inline static FName PoseMovingCurve{TEXTVIEW("PoseMoving")};
	inline static FName PoseStandingCurve{TEXTVIEW("PoseStanding")};
	inline static FName PoseCrouchingCurve{TEXTVIEW("PoseCrouching")};
	inline static FName PoseGroundedCurve{TEXTVIEW("PoseGrounded")};
	inline static FName PoseInAirCurve{TEXTVIEW("PoseInAir")};

	// Feet Animation Curves

	inline static FName FootLeftIkCurve{TEXTVIEW("FootLeftIk")};
	inline static FName FootLeftLockCurve{TEXTVIEW("FootLeftLock")};
	inline static FName FootRightIkCurve{TEXTVIEW("FootRightIk")};
	inline static FName FootRightLockCurve{TEXTVIEW("FootRightLock")};
	inline static FName FootPlantedCurve{TEXTVIEW("FootPlanted")};
	inline static FName FeetCrossingCurve{TEXTVIEW("FeetCrossing")};

	// Other Animation Curves

	inline static FName RotationYawSpeedCurve{TEXTVIEW("RotationYawSpeed")};
	inline static FName RotationYawOffsetCurve{TEXTVIEW("RotationYawOffset")};
	inline static FName AllowTransitionsCurve{TEXTVIEW("AllowTransitions")};
	inline static FName SprintBlockCurve{TEXTVIEW("SprintBlock")};
	inline static FName GroundPredictionBlockCurve{TEXTVIEW("GroundPredictionBlock")};
	inline static FName FootstepSoundBlockCurve{TEXTVIEW("FootstepSoundBlock")};

	// Debug

	inline static FName CurvesDebugDisplay{TEXTVIEW("ALS.Curves")};
	inline static FName StateDebugDisplay{TEXTVIEW("ALS.State")};
	inline static FName ShapesDebugDisplay{TEXTVIEW("ALS.Shapes")};
	inline static FName TracesDebugDisplay{TEXTVIEW("ALS.Traces")};
	inline static FName MantlingDebugDisplay{TEXTVIEW("ALS.Mantling")};

public:
	// Bones

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName RootBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName PelvisBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName HeadBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName Spine03BoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName FootLeftBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName FootRightBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName HandLeftGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName HandRightGunVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName FootLeftIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName FootRightIkBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName FootLeftVirtualBoneName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Bones", Meta = (ReturnDisplayName = "Bone Name"))
	static FName FootRightVirtualBoneName();

	// Animation Slots

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static FName TransitionSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static FName TurnInPlaceStandingSlotName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Slots", Meta = (ReturnDisplayName = "Slot Name"))
	static FName TurnInPlaceCrouchingSlotName();

	// Layering Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerHeadCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerHeadAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerHeadSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmLeftAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmLeftLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmLeftSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmRightAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmRightLocalSpaceCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerArmRightSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerHandLeftCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerHandRightCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerSpineCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerSpineAdditiveCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerSpineSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerPelvisCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerPelvisSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerLegsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName LayerLegsSlotCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName HandLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName HandRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName ViewBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName AllowAimingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName HipsDirectionLockCurveName();

	// Pose Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PoseGaitCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PoseMovingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PoseStandingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PoseCrouchingCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PoseGroundedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName PoseInAirCurveName();

	// Feet Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FootLeftIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FootLeftLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FootRightIkCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FootRightLockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FootPlantedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FeetCrossingCurveName();

	// Other Animation Curves

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName RotationYawSpeedCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName RotationYawOffsetCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName AllowTransitionsCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName SprintBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName GroundPredictionBlockCurveName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Animation Curves", Meta = (ReturnDisplayName = "Curve Name"))
	static FName FootstepSoundBlockCurveName();

	// Debug

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName CurvesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName StateDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName ShapesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName TracesDebugDisplayName();

	UFUNCTION(BlueprintPure, Category = "ALS|Constants|Debug", Meta = (ReturnDisplayName = "Display Name"))
	static FName MantlingDebugDisplayName();
};

inline FName UAlsConstants::RootBoneName()
{
	return RootBone;
}

inline FName UAlsConstants::PelvisBoneName()
{
	return PelvisBone;
}

inline FName UAlsConstants::HeadBoneName()
{
	return HeadBone;
}

inline FName UAlsConstants::Spine03BoneName()
{
	return Spine03Bone;
}

inline FName UAlsConstants::FootLeftBoneName()
{
	return FootLeftBone;
}

inline FName UAlsConstants::FootRightBoneName()
{
	return FootRightBone;
}

inline FName UAlsConstants::HandLeftGunVirtualBoneName()
{
	return HandLeftGunVirtualBone;
}

inline FName UAlsConstants::HandRightGunVirtualBoneName()
{
	return HandRightGunVirtualBone;
}

inline FName UAlsConstants::FootLeftIkBoneName()
{
	return FootLeftIkBone;
}

inline FName UAlsConstants::FootRightIkBoneName()
{
	return FootRightIkBone;
}

inline FName UAlsConstants::FootLeftVirtualBoneName()
{
	return FootLeftVirtualBone;
}

inline FName UAlsConstants::FootRightVirtualBoneName()
{
	return FootRightVirtualBone;
}

inline FName UAlsConstants::TransitionSlotName()
{
	return TransitionSlot;
}

inline FName UAlsConstants::TurnInPlaceStandingSlotName()
{
	return TurnInPlaceStandingSlot;
}

inline FName UAlsConstants::TurnInPlaceCrouchingSlotName()
{
	return TurnInPlaceCrouchingSlot;
}

inline FName UAlsConstants::LayerHeadCurveName()
{
	return LayerHeadCurve;
}

inline FName UAlsConstants::LayerHeadAdditiveCurveName()
{
	return LayerHeadAdditiveCurve;
}

inline FName UAlsConstants::LayerHeadSlotCurveName()
{
	return LayerHeadSlotCurve;
}

inline FName UAlsConstants::LayerArmLeftCurveName()
{
	return LayerArmLeftCurve;
}

inline FName UAlsConstants::LayerArmLeftAdditiveCurveName()
{
	return LayerArmLeftAdditiveCurve;
}

inline FName UAlsConstants::LayerArmLeftLocalSpaceCurveName()
{
	return LayerArmLeftLocalSpaceCurve;
}

inline FName UAlsConstants::LayerArmLeftSlotCurveName()
{
	return LayerArmLeftSlotCurve;
}

inline FName UAlsConstants::LayerArmRightCurveName()
{
	return LayerArmRightCurve;
}

inline FName UAlsConstants::LayerArmRightAdditiveCurveName()
{
	return LayerArmRightAdditiveCurve;
}

inline FName UAlsConstants::LayerArmRightLocalSpaceCurveName()
{
	return LayerArmRightLocalSpaceCurve;
}

inline FName UAlsConstants::LayerArmRightSlotCurveName()
{
	return LayerArmRightSlotCurve;
}

inline FName UAlsConstants::LayerHandLeftCurveName()
{
	return LayerHandLeftCurve;
}

inline FName UAlsConstants::LayerHandRightCurveName()
{
	return LayerHandRightCurve;
}

inline FName UAlsConstants::LayerSpineCurveName()
{
	return LayerSpineCurve;
}

inline FName UAlsConstants::LayerSpineAdditiveCurveName()
{
	return LayerSpineAdditiveCurve;
}

inline FName UAlsConstants::LayerSpineSlotCurveName()
{
	return LayerSpineSlotCurve;
}

inline FName UAlsConstants::LayerPelvisCurveName()
{
	return LayerPelvisCurve;
}

inline FName UAlsConstants::LayerPelvisSlotCurveName()
{
	return LayerPelvisSlotCurve;
}

inline FName UAlsConstants::LayerLegsCurveName()
{
	return LayerLegsCurve;
}

inline FName UAlsConstants::LayerLegsSlotCurveName()
{
	return LayerLegsSlotCurve;
}

inline FName UAlsConstants::HandLeftIkCurveName()
{
	return HandLeftIkCurve;
}

inline FName UAlsConstants::HandRightIkCurveName()
{
	return HandRightIkCurve;
}

inline FName UAlsConstants::ViewBlockCurveName()
{
	return ViewBlockCurve;
}

inline FName UAlsConstants::AllowAimingCurveName()
{
	return AllowAimingCurve;
}

inline FName UAlsConstants::HipsDirectionLockCurveName()
{
	return HipsDirectionLockCurve;
}

inline FName UAlsConstants::PoseGaitCurveName()
{
	return PoseGaitCurve;
}

inline FName UAlsConstants::PoseMovingCurveName()
{
	return PoseMovingCurve;
}

inline FName UAlsConstants::PoseStandingCurveName()
{
	return PoseStandingCurve;
}

inline FName UAlsConstants::PoseCrouchingCurveName()
{
	return PoseCrouchingCurve;
}

inline FName UAlsConstants::PoseGroundedCurveName()
{
	return PoseGroundedCurve;
}

inline FName UAlsConstants::PoseInAirCurveName()
{
	return PoseInAirCurve;
}

inline FName UAlsConstants::FootLeftIkCurveName()
{
	return FootLeftIkCurve;
}

inline FName UAlsConstants::FootLeftLockCurveName()
{
	return FootLeftLockCurve;
}

inline FName UAlsConstants::FootRightIkCurveName()
{
	return FootRightIkCurve;
}

inline FName UAlsConstants::FootRightLockCurveName()
{
	return FootRightLockCurve;
}

inline FName UAlsConstants::FootPlantedCurveName()
{
	return FootPlantedCurve;
}

inline FName UAlsConstants::FeetCrossingCurveName()
{
	return FeetCrossingCurve;
}

inline FName UAlsConstants::RotationYawSpeedCurveName()
{
	return RotationYawSpeedCurve;
}

inline FName UAlsConstants::RotationYawOffsetCurveName()
{
	return RotationYawOffsetCurve;
}

inline FName UAlsConstants::AllowTransitionsCurveName()
{
	return AllowTransitionsCurve;
}

inline FName UAlsConstants::SprintBlockCurveName()
{
	return SprintBlockCurve;
}

inline FName UAlsConstants::GroundPredictionBlockCurveName()
{
	return GroundPredictionBlockCurve;
}

inline FName UAlsConstants::FootstepSoundBlockCurveName()
{
	return FootstepSoundBlockCurve;
}

inline FName UAlsConstants::CurvesDebugDisplayName()
{
	return CurvesDebugDisplay;
}

inline FName UAlsConstants::StateDebugDisplayName()
{
	return StateDebugDisplay;
}

inline FName UAlsConstants::ShapesDebugDisplayName()
{
	return ShapesDebugDisplay;
}

inline FName UAlsConstants::TracesDebugDisplayName()
{
	return TracesDebugDisplay;
}

inline FName UAlsConstants::MantlingDebugDisplayName()
{
	return MantlingDebugDisplay;
}
