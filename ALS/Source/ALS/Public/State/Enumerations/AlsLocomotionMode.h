#pragma once

#include "AlsLocomotionMode.generated.h"

UENUM(BlueprintType)
enum class EAlsLocomotionMode : uint8
{
	Grounded,
	InAir,
	Mantling,
	Ragdolling
};
