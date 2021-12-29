#include "Settings/AlsAnimationInstanceSettings.h"

#include "Engine/CollisionProfile.h"

UAlsAnimationInstanceSettings::UAlsAnimationInstanceSettings()
{
	InAir.GroundPredictionSweepObjectTypes =
	{
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};
}
