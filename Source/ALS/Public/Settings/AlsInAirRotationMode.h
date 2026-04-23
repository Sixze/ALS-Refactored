#pragma once

#include "AlsInAirRotationMode.generated.h"

UENUM(BlueprintType)
enum class EAlsInAirRotationMode : uint8
{
	/// Character will not rotate after entering in air mode.
	KeepWorldRotation,

	/// Character will retain the last rotation at the moment of entering in
	/// air mode, but will follow the camera rotation as the player rotates it.
	KeepViewSpaceRotation,

	/// Character will rotate in the direction of its velocity.
	RotateToVelocity
};
