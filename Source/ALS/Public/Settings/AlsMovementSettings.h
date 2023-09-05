#pragma once

#include "Engine/DataAsset.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsMovementSettings.generated.h"

class UCurveFloat;
class UCurveVector;

USTRUCT(BlueprintType)
struct ALS_API FAlsMovementGaitSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float WalkSpeed{175.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float RunSpeed{375.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float SprintSpeed{650.0f};

	// Gait amount to acceleration, deceleration, and ground friction curve.
	// Gait amount ranges from 0 to 3, where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveVector> AccelerationAndDecelerationAndGroundFrictionCurve{nullptr};

	// Gait amount to rotation interpolation speed curve.
	// Gait amount ranges from 0 to 3, where 0 is stopped, 1 is walking, 2 is running, and 3 is sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TObjectPtr<UCurveFloat> RotationInterpolationSpeedCurve{nullptr};

public:
	float GetSpeedByGait(const FGameplayTag& Gait) const;
};

USTRUCT(BlueprintType)
struct ALS_API FAlsMovementStanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ForceInlineRow))
	TMap<FGameplayTag, FAlsMovementGaitSettings> Stances
	{
		{AlsStanceTags::Standing, {}},
		{AlsStanceTags::Crouching, {}}
	};
};

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsMovementSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ForceInlineRow))
	TMap<FGameplayTag, FAlsMovementStanceSettings> RotationModes
	{
		{AlsRotationModeTags::VelocityDirection, {}},
		{AlsRotationModeTags::ViewDirection, {}},
		{AlsRotationModeTags::Aiming, {}}
	};
};

inline float FAlsMovementGaitSettings::GetSpeedByGait(const FGameplayTag& Gait) const
{
	if (Gait == AlsGaitTags::Walking)
	{
		return WalkSpeed;
	}

	if (Gait == AlsGaitTags::Running)
	{
		return RunSpeed;
	}

	if (Gait == AlsGaitTags::Sprinting)
	{
		return SprintSpeed;
	}

	return 0.0f;
}
