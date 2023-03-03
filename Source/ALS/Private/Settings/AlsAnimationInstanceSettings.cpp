#include "Settings/AlsAnimationInstanceSettings.h"

#include "Engine/CollisionProfile.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationInstanceSettings)

UAlsAnimationInstanceSettings::UAlsAnimationInstanceSettings()
{
	InAir.GroundPredictionSweepObjectTypes =
	{
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};
}
