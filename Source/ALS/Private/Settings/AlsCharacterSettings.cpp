#include "Settings/AlsCharacterSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsCharacterSettings)

UAlsCharacterSettings::UAlsCharacterSettings()
{
	Ragdolling.GroundTraceResponseChannels =
	{
		ECC_WorldStatic,
		ECC_WorldDynamic,
		ECC_Destructible
	};

	Ragdolling.GroundTraceResponses.WorldStatic = ECR_Block;
	Ragdolling.GroundTraceResponses.WorldDynamic = ECR_Block;
	Ragdolling.GroundTraceResponses.Destructible = ECR_Block;

	Mantling.MantlingTraceResponseChannels =
	{
		ECC_WorldStatic,
		ECC_WorldDynamic,
		ECC_Destructible
	};

	Mantling.MantlingTraceResponses.WorldStatic = ECR_Block;
	Mantling.MantlingTraceResponses.WorldDynamic = ECR_Block;
	Mantling.MantlingTraceResponses.Destructible = ECR_Block;
}

#if WITH_EDITOR
void UAlsCharacterSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Ragdolling.PostEditChangeProperty(PropertyChangedEvent);
	Mantling.PostEditChangeProperty(PropertyChangedEvent);

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
