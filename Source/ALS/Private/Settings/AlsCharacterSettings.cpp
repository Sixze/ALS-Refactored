#include "Settings/AlsCharacterSettings.h"

#include "Engine/CollisionProfile.h"

UAlsCharacterSettings::UAlsCharacterSettings()
{
	Ragdolling.GroundTraceObjectTypes =
	{
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};

	Mantling.MantlingTraceObjectTypes =
	{
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};
}
