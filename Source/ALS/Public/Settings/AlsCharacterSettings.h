#pragma once

#include "AlsInAirRotationMode.h"
#include "AlsMantlingSettings.h"
#include "AlsRagdollingSettings.h"
#include "AlsRollingSettings.h"
#include "AlsViewSettings.h"
#include "AlsCharacterSettings.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsCharacterSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	/// Character is considered as teleported if they move farther than this distance within one frame.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TeleportDistanceThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float MovingSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EAlsInAirRotationMode InAirRotationMode{EAlsInAirRotationMode::RotateToVelocityOnJump};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bAllowAimingWhenInAir : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bSprintHasPriorityOverAiming : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bRotateToVelocityWhenSprinting : 1 {false};

	/// In the velocity direction rotation mode, this setting makes the character rotate relative to the
	/// object they are standing on. Otherwise, the character ignores that object and keeps its world rotation.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bInheritMovementBaseRotationInVelocityDirectionRotationMode : 1 {false};

	/// Makes the character rotate in the direction they want to move, but they are not always able to do so due to obstacles.
	/// This setting is only used if the bIgnoreBaseRotation setting from the character movement component is set to true.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bRotateTowardsDesiredVelocityInVelocityDirectionRotationMode : 1 {true};

	/// Makes the character automatically rotate towards the view direction on any movement input while the actor is not moving.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bAutoRotateOnAnyInputWhileNotMovingInViewDirectionRotationMode : 1 {true};

	/// Indicates how much the character's yaw angle can differ from the view yaw angle when
	/// aiming or in first-person mode. Use it to prevent the lower body from noticeably failing
	/// to keep up with the rotation of the upper body when the camera rotates very quickly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float AimingYawAngleLimit{70.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsViewSettings View;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsGeneralMantlingSettings Mantling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsRagdollingSettings Ragdolling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsRollingSettings Rolling;

public:
	UAlsCharacterSettings();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;
#endif
};
