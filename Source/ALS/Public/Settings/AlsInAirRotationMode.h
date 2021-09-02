#pragma once

#include "AlsInAirRotationMode.generated.h"

UENUM(BlueprintType)
enum class EAlsInAirRotationMode : uint8
{
	RotateToVelocityOnJump,
	KeepRelativeRotation,
	KeepWorldRotation
};
