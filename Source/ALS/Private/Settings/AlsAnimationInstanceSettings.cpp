#include "Settings/AlsAnimationInstanceSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimationInstanceSettings)

UAlsAnimationInstanceSettings::UAlsAnimationInstanceSettings()
{
	InAir.GroundPredictionSweepObjectTypes =
	{
		UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECC_Destructible)
	};

	InAir.GroundPredictionSweepResponses.SetResponse(ECC_WorldStatic, ECR_Block);
	InAir.GroundPredictionSweepResponses.SetResponse(ECC_WorldDynamic, ECR_Block);
	InAir.GroundPredictionSweepResponses.SetResponse(ECC_Destructible, ECR_Block);
}

#if WITH_EDITOR
void UAlsAnimationInstanceSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	InAir.PostEditChangeProperty(PropertyChangedEvent);

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
