#include "Settings/AlsCharacterSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsCharacterSettings)

UAlsCharacterSettings::UAlsCharacterSettings()
{
	Ragdolling.GroundTraceObjectTypes =
	{
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECC_Destructible)
	};

	Ragdolling.GroundTraceResponses.SetResponse(ECC_WorldStatic, ECR_Block);
	Ragdolling.GroundTraceResponses.SetResponse(ECC_WorldDynamic, ECR_Block);
	Ragdolling.GroundTraceResponses.SetResponse(ECC_Destructible, ECR_Block);

	Mantling.MantlingTraceObjectTypes =
	{
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECC_Destructible)
	};

	Mantling.MantlingTraceResponses.SetResponse(ECC_WorldStatic, ECR_Block);
	Mantling.MantlingTraceResponses.SetResponse(ECC_WorldDynamic, ECR_Block);
	Mantling.MantlingTraceResponses.SetResponse(ECC_Destructible, ECR_Block);
}

#if WITH_EDITOR
void UAlsCharacterSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Ragdolling.PostEditChangeProperty(PropertyChangedEvent);
	Mantling.PostEditChangeProperty(PropertyChangedEvent);

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
