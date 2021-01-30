#pragma once

#include "AlsRotationMode.generated.h"

UENUM(BlueprintType)
enum class EAlsRotationMode : uint8
{
	LookingDirection,
	VelocityDirection,
	Aiming
};
