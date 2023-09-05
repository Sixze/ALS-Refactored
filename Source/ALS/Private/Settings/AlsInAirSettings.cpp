#include "Settings/AlsInAirSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsInAirSettings)

#if WITH_EDITOR
void FAlsInAirSettings::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() != GET_MEMBER_NAME_CHECKED(FAlsInAirSettings, GroundPredictionResponseChannels))
	{
		return;
	}

	GroundPredictionSweepResponses.SetAllChannels(ECR_Ignore);

	for (const auto CollisionChannel : GroundPredictionResponseChannels)
	{
		GroundPredictionSweepResponses.SetResponse(CollisionChannel, ECR_Block);
	}
}
#endif
