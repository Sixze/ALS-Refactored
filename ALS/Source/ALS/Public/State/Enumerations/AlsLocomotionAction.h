#pragma once

#include "AlsLocomotionAction.generated.h"

UENUM(BlueprintType)
enum class EAlsLocomotionAction : uint8
{
	None,
	Mantling,
	Rolling,
	GettingUp
};
