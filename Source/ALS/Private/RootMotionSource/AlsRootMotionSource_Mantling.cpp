#include "RootMotionSource/AlsRootMotionSource_Mantling.h"

#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "Settings/AlsMantlingSettings.h"
#include "Utility/AlsMacro.h"

FAlsRootMotionSource_Mantling::FAlsRootMotionSource_Mantling()
{
	Priority = 1000;
}

FRootMotionSource* FAlsRootMotionSource_Mantling::Clone() const
{
	return new FAlsRootMotionSource_Mantling{*this};
}

bool FAlsRootMotionSource_Mantling::Matches(const FRootMotionSource* Other) const
{
	if (!Super::Matches(Other))
	{
		return false;
	}

	const auto* OtherCasted{static_cast<const FAlsRootMotionSource_Mantling*>(Other)};

	return MantlingSettings == OtherCasted->MantlingSettings &&
	       TargetPrimitive == OtherCasted->TargetPrimitive;
}

void FAlsRootMotionSource_Mantling::PrepareRootMotion(const float SimulationDeltaTime, const float DeltaTime,
                                                      const ACharacter& Character, const UCharacterMovementComponent& Movement)
{
	SetTime(GetTime() + SimulationDeltaTime);

	if (!ensure(Duration > SMALL_NUMBER) || DeltaTime <= SMALL_NUMBER)
	{
		RootMotionParams.Clear();
		return;
	}

	const auto MantlingTime{GetTime() * MantlingSettings->CalculatePlayRate(MantlingHeight)};

	// Calculate target transform from the stored relative transform to follow along with moving objects.

	auto TargetTransform{
		TargetPrimitive.IsValid()
			? FTransform{TargetRelativeRotation, TargetRelativeLocation, TargetPrimitive->GetComponentScale()}
			.GetRelativeTransformReverse(TargetPrimitive->GetComponentTransform())
			: FTransform{TargetRelativeRotation, TargetRelativeLocation}
	};

	FVector LocationOffset;
	FRotator RotationOffset;

	const auto BlendInAmount{MantlingSettings->BlendInCurve->GetFloatValue(MantlingTime)};
	if (BlendInAmount <= KINDA_SMALL_NUMBER)
	{
		LocationOffset = ActorLocationOffset;
		RotationOffset = ActorRotationOffset;
	}
	else
	{
		const auto InterpolationAndCorrectionAmounts{
			MantlingSettings->InterpolationAndCorrectionAmountsCurve->GetVectorValue(
				MantlingTime + MantlingSettings->CalculateStartTime(MantlingHeight))
		};

		const auto InterpolationAmount{InterpolationAndCorrectionAmounts.X};
		const auto HorizontalCorrectionAmount{InterpolationAndCorrectionAmounts.Y};
		const auto VerticalCorrectionAmount{InterpolationAndCorrectionAmounts.Z};

		if (InterpolationAmount <= KINDA_SMALL_NUMBER)
		{
			LocationOffset = FVector::ZeroVector;
			RotationOffset = FRotator::ZeroRotator;
		}
		else
		{
			// Calculate the animation offset. This would be the location the actual animation starts at relative to the target transform.

			auto AnimationLocationOffset{TargetTransform.GetUnitAxis(EAxis::X) * MantlingSettings->StartRelativeLocation.X};
			AnimationLocationOffset.Z = MantlingSettings->StartRelativeLocation.Z;

			// Blend into the animation offset and final offset at the same time.
			// Horizontal and vertical blends use different correction amounts.

			LocationOffset.X = FMath::Lerp(ActorLocationOffset.X, AnimationLocationOffset.X, HorizontalCorrectionAmount) *
			                   InterpolationAmount;
			LocationOffset.Y = FMath::Lerp(ActorLocationOffset.Y, AnimationLocationOffset.Y, HorizontalCorrectionAmount) *
			                   InterpolationAmount;
			LocationOffset.Z = FMath::Lerp(ActorLocationOffset.Z, AnimationLocationOffset.Z, VerticalCorrectionAmount) *
			                   InterpolationAmount;

			// Actor rotation offset must be normalized for this block of code to work properly.

			RotationOffset = ActorRotationOffset * (1.0f - HorizontalCorrectionAmount) * InterpolationAmount;
		}

		// Initial blend in allows the actor to blend into the interpolation and correction curves at
		// the midpoint. This prevents pops when mantling an object lower than the animated mantling.

		if (BlendInAmount < 1.0f - KINDA_SMALL_NUMBER)
		{
			LocationOffset = FMath::Lerp<FVector>(ActorLocationOffset, LocationOffset, BlendInAmount);
			RotationOffset = FMath::Lerp(ActorRotationOffset, RotationOffset, BlendInAmount);
		}
	}

	// Apply final offsets.

	TargetTransform.AddToTranslation(LocationOffset);
	TargetTransform.ConcatenateRotation(RotationOffset.Quaternion());

	// Find the delta transform between the character and the target transform and divide by the delta time to get the velocity.

	TargetTransform.AddToTranslation(-Character.GetActorLocation());
	TargetTransform.ConcatenateRotation(Character.GetActorQuat().Inverse());

	RootMotionParams.Set(TargetTransform * ScalarRegister{1.0f / DeltaTime});
	bSimulatedNeedsSmoothing = true;
}

bool FAlsRootMotionSource_Mantling::NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bSuccess)
{
	if (!Super::NetSerialize(Archive, Map, bSuccess))
	{
		return false;
	}

	auto bSuccessLocal{true};

	Archive << MantlingSettings;
	Archive << TargetPrimitive;

	TargetRelativeLocation.NetSerialize(Archive, Map, bSuccessLocal);
	bSuccess &= bSuccessLocal;

	TargetRelativeRotation.NetSerialize(Archive, Map, bSuccessLocal);
	bSuccess &= bSuccessLocal;

	ActorLocationOffset.NetSerialize(Archive, Map, bSuccessLocal);
	bSuccess &= bSuccessLocal;

	ActorRotationOffset.NetSerialize(Archive, Map, bSuccessLocal);
	bSuccess &= bSuccessLocal;

	Archive << MantlingHeight;

	return true;
}

UScriptStruct* FAlsRootMotionSource_Mantling::GetScriptStruct() const
{
	return StaticStruct();
}

FString FAlsRootMotionSource_Mantling::ToSimpleString() const
{
	return FString::Format(TEXT("{0} ({1}, {2})"), {GET_TYPE_STRING(FAlsRootMotionSource_Mantling), *InstanceName.ToString(), LocalID});
}

void FAlsRootMotionSource_Mantling::AddReferencedObjects(class FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(MantlingSettings);
}
