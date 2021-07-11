#include "AlsCameraComponent.h"

#include "AlsCameraAnimationInstance.h"
#include "AlsCharacter.h"
#include "Camera/CameraTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utility/AlsCameraConstants.h"
#include "Utility/AlsMath.h"

UAlsCameraComponent::UAlsCameraComponent()
{
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	bTickInEditor = false;
	bHiddenInGame = true;

	AnimClass = UAlsCameraAnimationInstance::StaticClass();
}

void UAlsCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	check(IsValid(SkeletalMesh))

	AlsCharacter = Cast<AAlsCharacter>(GetOwner());
	check(IsValid(AlsCharacter))

	TickCamera(0.0f, false);
}

void UAlsCameraComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCamera(DeltaTime);
}

FVector UAlsCameraComponent::GetFirstPersonPivotLocation() const
{
	return AlsCharacter->GetMesh()->GetSocketLocation(FirstPersonSocket);
}

FTransform UAlsCameraComponent::GetThirdPersonPivotTransform() const
{
	return {
		GetComponentRotation(),
		(AlsCharacter->GetMesh()->GetSocketLocation(ThirdPersonFirstSocket) +
		 AlsCharacter->GetMesh()->GetSocketLocation(ThirdPersonSecondSocket)) * 0.5f
	};
}

void UAlsCameraComponent::GetViewInfo(FMinimalViewInfo& ViewInfo) const
{
	ViewInfo.Location = PreviousLocation;
	ViewInfo.Rotation = PreviousRotation;
	ViewInfo.FOV = PreviousFov;
}

void UAlsCameraComponent::TickCamera(float DeltaTime, bool bAllowLag)
{
	// Calculate result rotation. Use the view rotation and interpolate for smooth rotation.

	FQuat ResultRotation;

	if (bAllowLag)
	{
		ResultRotation = FQuat::Slerp(PreviousRotation.Quaternion(), AlsCharacter->GetViewRotation().Quaternion(),
		                              UAlsMath::ExponentialDecayDamp(
			                              GetAnimInstance()->GetCurveValue(UAlsCameraConstants::RotationLagCurve()),
			                              DeltaTime));
	}
	else
	{
		ResultRotation = AlsCharacter->GetViewRotation().Quaternion();
	}

	PreviousRotation = ResultRotation.Rotator();

	const auto FirstPersonAmount{UAlsMath::Clamp01(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::FirstPersonAmountCurve()))};
	if (FirstPersonAmount >= 1.0f)
	{
		PreviousPivotLocation = GetThirdPersonPivotTransform().GetLocation();
		PreviousLocation = GetFirstPersonPivotLocation();
		PreviousFov = FirstPersonFov;
		return;
	}

	// Calculate pivot location. Get the target pivot transform and
	// interpolate using axis independent lag for maximum control.

	const auto TargetPivotTransform{GetThirdPersonPivotTransform()};

	if (bAllowLag)
	{
		const auto LocationLagRotation{FRotator{0.0f, PreviousRotation.Yaw, 0.0f}.Quaternion()};
		const auto RelativePreviousPivotLocation{LocationLagRotation.UnrotateVector(PreviousPivotLocation)};
		const auto RelativeTargetPivotLocation{LocationLagRotation.UnrotateVector(TargetPivotTransform.GetLocation())};

		PreviousPivotLocation = LocationLagRotation.RotateVector({
			FMath::Lerp(RelativePreviousPivotLocation.X, RelativeTargetPivotLocation.X,
			            UAlsMath::ExponentialDecayDamp(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagXCurve()),
			                                           DeltaTime)),
			FMath::Lerp(RelativePreviousPivotLocation.Y, RelativeTargetPivotLocation.Y,
			            UAlsMath::ExponentialDecayDamp(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagYCurve()),
			                                           DeltaTime)),
			FMath::Lerp(RelativePreviousPivotLocation.Z, RelativeTargetPivotLocation.Z,
			            UAlsMath::ExponentialDecayDamp(GetAnimInstance()->GetCurveValue(UAlsCameraConstants::LocationLagZCurve()),
			                                           DeltaTime))
		});
	}
	else
	{
		PreviousPivotLocation = TargetPivotTransform.GetLocation();
	}

	// Calculate result location. Get the pivot location, apply local
	// offsets for further camera control, and apply camera relative offsets.

	auto ResultLocation{
		PreviousPivotLocation +
		TargetPivotTransform.TransformVectorNoScale({
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetXCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetYCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::PivotOffsetZCurve())
		}) +
		ResultRotation.RotateVector({
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetXCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetYCurve()),
			GetAnimInstance()->GetCurveValue(UAlsCameraConstants::CameraOffsetZCurve())
		})
	};

	// Trace for an object between the camera and character to apply a corrective offset.

	auto TraceStart{
		AlsCharacter->GetMesh()->GetSocketLocation(bRightShoulder ? ThirdPersonTraceRightSocket : ThirdPersonTraceLeftSocket)
	};

	FHitResult Hit;
	GetWorld()->SweepSingleByChannel(Hit, TraceStart, ResultLocation, FQuat::Identity,
	                                 UEngineTypes::ConvertToCollisionChannel(ThirdPersonTraceChannel),
	                                 FCollisionShape::MakeSphere(ThirdPersonTraceRadius),
	                                 {__FUNCTION__, false, GetOwner()});

	if (Hit.IsValidBlockingHit())
	{
		ResultLocation = Hit.Location;
	}

	if (FirstPersonAmount <= 0.0f)
	{
		PreviousLocation = ResultLocation;
		PreviousFov = ThirdPersonFov;
	}
	else
	{
		PreviousLocation = FMath::Lerp(ResultLocation, GetFirstPersonPivotLocation(), FirstPersonAmount);
		PreviousFov = FMath::Lerp(ThirdPersonFov, FirstPersonFov, FirstPersonAmount);
	}
}
