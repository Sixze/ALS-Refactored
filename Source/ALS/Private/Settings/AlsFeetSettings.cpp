#include "Settings/AlsFeetSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsFeetSettings)

#if WITH_EDITOR
void FAlsFootLimitsSettings::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent)
{
	TwistAxis.Normalize();
	TwistLimit = FMath::Sin(FMath::DegreesToRadians(TwistLimitAngle) * 0.5f);

	SwingLimitOffsetQuaternion = SwingLimitOffset.Quaternion();
	Swing1Limit = FMath::Sin(FMath::DegreesToRadians(Swing1LimitAngle) * 0.5f);
	Swing2Limit = FMath::Sin(FMath::DegreesToRadians(Swing2LimitAngle) * 0.5f);
}

void FAlsFeetSettings::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent)
{
	LeftFootLimits.PostEditChangeProperty(PropertyChangedEvent);
	RightFootLimits.PostEditChangeProperty(PropertyChangedEvent);
}
#endif
