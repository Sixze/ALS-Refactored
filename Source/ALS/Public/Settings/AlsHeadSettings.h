#pragma once

#include "AlsHeadSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsHeadSettings
{
	GENERATED_BODY()

	/// The lower the value, the faster the interpolation. A zero value means instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float PitchAngleInterpolationHalfLife{0.1f};

	/// The lower the value, the faster the interpolation. A zero value means instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float YawAngleInterpolationHalfLife{0.1f};

	/// The lower the value, the faster the interpolation. A zero value means instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float SwitchLookSidesYawAngleInterpolationHalfLife{0.2f};

	/// The lower the value, the faster the interpolation. A zero value means instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float FirstPersonPitchAngleInterpolationHalfLife{0.01f};

	/// The lower the value, the faster the interpolation. A zero value means instant interpolation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float FirstPersonYawAngleInterpolationHalfLife{0.01f};
};
