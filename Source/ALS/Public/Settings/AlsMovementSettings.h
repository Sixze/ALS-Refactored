#pragma once

#include "Engine/DataAsset.h"
#include "State/Enumerations/AlsGait.h"
#include "State/Enumerations/AlsRotationMode.h"
#include "State/Enumerations/AlsStance.h"
#include "AlsMovementSettings.generated.h"

class UCurveFloat;
class UCurveVector;

USTRUCT(BlueprintType)
struct ALS_API FAlsMovementGaitSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float WalkSpeed{175.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float RunSpeed{375.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float SprintSpeed{650.0f};

	// Gait amount to acceleration, deceleration and ground friction curve.
	// Gait amount ranges from 0 to 3, where 0 is stopped, 1 is walking, 2 is running and 3 is sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveVector> AccelerationAndDecelerationAndGroundFrictionCurve{nullptr};

	// Gait amount to rotation interpolation speed curve.
	// Gait amount ranges from 0 to 3, where 0 is stopped, 1 is walking, 2 is running and 3 is sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> RotationInterpolationSpeedCurve{nullptr};

public:
	float GetSpeedForGait(const EAlsGait Gait) const;
};

inline float FAlsMovementGaitSettings::GetSpeedForGait(const EAlsGait Gait) const
{
	switch (Gait)
	{
		case EAlsGait::Running:
			return RunSpeed;

		case EAlsGait::Sprinting:
			return SprintSpeed;

		case EAlsGait::Walking:
			return WalkSpeed;

		default:
			checkNoEntry();
			return 0.0f;
	}
}

USTRUCT(BlueprintType)
struct ALS_API FAlsMovementStanceSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMovementGaitSettings Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAlsMovementGaitSettings Crouching;

public:
	const FAlsMovementGaitSettings* GetMovementGaitSettingsForStance(const EAlsStance Stance) const;
};

inline const FAlsMovementGaitSettings* FAlsMovementStanceSettings::GetMovementGaitSettingsForStance(const EAlsStance Stance) const
{
	switch (Stance)
	{
		case EAlsStance::Standing:
			return &Standing;

		case EAlsStance::Crouching:
			return &Crouching;

		default:
			checkNoEntry();
			return nullptr;
	}
}

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsMovementSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsMovementStanceSettings LookingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsMovementStanceSettings VelocityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAlsMovementStanceSettings Aiming;

public:
	const FAlsMovementStanceSettings* GetMovementStanceSettingsForRotationMode(const EAlsRotationMode RotationMode) const;
};

inline const FAlsMovementStanceSettings* UAlsMovementSettings::GetMovementStanceSettingsForRotationMode(
	const EAlsRotationMode RotationMode) const
{
	switch (RotationMode)
	{
		case EAlsRotationMode::LookingDirection:
			return &LookingDirection;

		case EAlsRotationMode::VelocityDirection:
			return &VelocityDirection;

		case EAlsRotationMode::Aiming:
			return &Aiming;

		default:
			checkNoEntry();
			return nullptr;
	}
}
