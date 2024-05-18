#include "Settings/AlsFeetSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsFeetSettings)

#if WITH_EDITOR
void FAlsFootConstraintsSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	TwistAxis.Normalize();
	TwistLimit = FMath::Sin(FMath::DegreesToRadians(TwistLimitAngle) * 0.5f);

	SwingLimitOffsetQuaternion = SwingLimitOffset.Quaternion();
	Swing1Limit = FMath::Sin(FMath::DegreesToRadians(Swing1LimitAngle) * 0.5f);
	Swing2Limit = FMath::Sin(FMath::DegreesToRadians(Swing2LimitAngle) * 0.5f);
}

void FAlsFeetSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	LeftFootConstraints.PostEditChangeProperty(ChangedEvent);
	RightFootConstraints.PostEditChangeProperty(ChangedEvent);
}
#endif
