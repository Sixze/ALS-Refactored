#include "Settings/AlsRagdollingSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRagdollingSettings)

#if WITH_EDITOR
void FAlsRagdollingSettings::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() != GET_MEMBER_NAME_CHECKED(FAlsRagdollingSettings, GroundTraceObjectTypes))
	{
		return;
	}

	GroundTraceResponses.SetAllChannels(ECR_Ignore);

	for (const auto ObjectType : GroundTraceObjectTypes)
	{
		GroundTraceResponses.SetResponse(UEngineTypes::ConvertToCollisionChannel(ObjectType), ECR_Block);
	}
}
#endif
