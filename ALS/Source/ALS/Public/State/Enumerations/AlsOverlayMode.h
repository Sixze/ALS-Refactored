#pragma once

#include "AlsOverlayMode.generated.h"

UENUM(BlueprintType)
enum class EAlsOverlayMode : uint8
{
	Default,
	Masculine,
	Feminine,
	Injured,
	HandsTied,
	M4,
	PistolOneHanded,
	PistolTwoHanded,
	Bow,
	Torch,
	Binoculars,
	Box,
	Barrel
};
