#include "Settings/AlsMovementSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsMovementSettings)

#if WITH_EDITOR
void UAlsMovementSettings::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, VelocityDirectionToSpeedInterpolationRange))
	{
		VelocityDirectionToSpeedInterpolationRange.Y = FMath::Min(VelocityDirectionToSpeedInterpolationRange.X,
		                                                          VelocityDirectionToSpeedInterpolationRange.Y);
	}

	Super::PostEditChangeProperty(ChangedEvent);
}
#endif
