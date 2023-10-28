#include "RootMotionSources/AlsRootMotionSource_Mantling.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMantlingSettings.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsRootMotionSource_Mantling)

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

	if (!ALS_ENSURE(GetDuration() > UE_SMALL_NUMBER) || DeltaTime <= UE_SMALL_NUMBER || !TargetPrimitive.IsValid())
	{
		RootMotionParams.Clear();
		return;
	}

	const auto* Montage{MantlingSettings->Montage.Get()};
	const auto MontageTime{MontageStartTime + GetTime() * Montage->RateScale};

	// Synchronize the mantling animation montage's time with the mantling root motion source's time.
	// Delta time subtraction is necessary here, otherwise there will be a one frame lag between them.

	Character.GetMesh()->GetAnimInstance()->Montage_SetPosition(Montage, FMath::Max(0.0f, MontageTime - DeltaTime));

	// Calculate the target transform based on the stored relative transform to follow moving objects.

	auto TargetTransform{
		MovementBaseUtility::UseRelativeLocation(TargetPrimitive.Get())
			? FTransform{TargetRelativeRotation, TargetRelativeLocation, TargetPrimitive->GetComponentScale()}
			.GetRelativeTransformReverse(TargetPrimitive->GetComponentTransform())
			: FTransform{TargetRelativeRotation, TargetRelativeLocation}
	};

	auto BlendInAmount{1.0f};

	const auto& MontageBlendIn{Montage->BlendIn};
	if (MontageBlendIn.GetBlendTime() > 0.0f)
	{
		BlendInAmount = FAlphaBlend::AlphaToBlendOption(GetTime() / MontageBlendIn.GetBlendTime(),
		                                                MontageBlendIn.GetBlendOption(), MontageBlendIn.GetCustomCurve());
	}

	const auto CurrentAnimationLocation{UAlsUtility::ExtractRootTransformFromMontage(Montage, MontageTime).GetLocation()};

	// The target animation location is expected to be non-zero, so it's safe to divide by it here.

	const auto InterpolationAmount{CurrentAnimationLocation.Z / TargetAnimationLocation.Z};

	if (!FAnimWeight::IsFullWeight(BlendInAmount * InterpolationAmount))
	{
		// Calculate the target animation location offset. This is the offset to
		// the location where the animation ends relative to the target transform.

		auto TargetAnimationLocationOffset{TargetTransform.GetUnitAxis(EAxis::X) * -TargetAnimationLocation.Y};
		TargetAnimationLocationOffset.Z = -TargetAnimationLocation.Z;
		TargetAnimationLocationOffset *= Character.GetMesh()->GetComponentScale().Z;

		// Blend into the animation offset and the final offset at the same time.
		// Horizontal and vertical blends use different correction amounts.

		auto HorizontalCorrectionAmount{1.0f};
		auto VerticalCorrectionAmount{1.0f};

		if (IsValid(MantlingSettings->HorizontalCorrectionCurve))
		{
			HorizontalCorrectionAmount = MantlingSettings->HorizontalCorrectionCurve->GetFloatValue(MontageTime);
		}

		if (IsValid(MantlingSettings->VerticalCorrectionCurve))
		{
			VerticalCorrectionAmount = MantlingSettings->VerticalCorrectionCurve->GetFloatValue(MontageTime);
		}

		FVector LocationOffset{
			FMath::Lerp(ActorFeetLocationOffset.X, TargetAnimationLocationOffset.X, HorizontalCorrectionAmount),
			FMath::Lerp(ActorFeetLocationOffset.Y, TargetAnimationLocationOffset.Y, HorizontalCorrectionAmount),
			FMath::Lerp(ActorFeetLocationOffset.Z, TargetAnimationLocationOffset.Z, VerticalCorrectionAmount)
		};

		LocationOffset = FMath::Lerp(ActorFeetLocationOffset, LocationOffset * (1.0f - InterpolationAmount), BlendInAmount);

		// The actor's rotation offset must be normalized for this code block to work properly.

		const auto RotationOffset{
			ActorRotationOffset *
			FMath::Lerp(1.0f, (1.0f - HorizontalCorrectionAmount) * (1.0f - InterpolationAmount), BlendInAmount)
		};

		// Apply final offsets.

		TargetTransform.AddToTranslation(LocationOffset);
		TargetTransform.ConcatenateRotation(RotationOffset.Quaternion());
	}
	else
	{
		Status.SetFlag(ERootMotionSourceStatusFlags::Finished);
	}

	// Find the delta transform between the actor and the target transform and divide it by the time delta to get the velocity.

	TargetTransform.AddToTranslation(-Movement.GetActorFeetLocation());
	TargetTransform.ConcatenateRotation(Movement.UpdatedComponent->GetComponentQuat().Inverse());

	RootMotionParams.Set(TargetTransform * ScalarRegister{1.0f / DeltaTime});
}

bool FAlsRootMotionSource_Mantling::NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bSuccess)
{
	if (!Super::NetSerialize(Archive, Map, bSuccess))
	{
		bSuccess = false;
		return false;
	}

	bSuccess = true;
	auto bSuccessLocal{true};

	Archive << MantlingSettings;
	Archive << TargetPrimitive;

	bSuccess &= SerializePackedVector<100, 30>(TargetRelativeLocation, Archive);

	TargetRelativeRotation.NetSerialize(Archive, Map, bSuccessLocal);
	TargetRelativeRotation.Normalize();
	bSuccess &= bSuccessLocal;

	bSuccess &= SerializePackedVector<100, 30>(ActorFeetLocationOffset, Archive);

	ActorRotationOffset.NetSerialize(Archive, Map, bSuccessLocal);
	ActorRotationOffset.Normalize();
	bSuccess &= bSuccessLocal;

	bSuccess &= SerializePackedVector<100, 30>(TargetAnimationLocation, Archive);

	Archive << MontageStartTime;

	return bSuccess;
}

UScriptStruct* FAlsRootMotionSource_Mantling::GetScriptStruct() const
{
	return StaticStruct();
}

FString FAlsRootMotionSource_Mantling::ToSimpleString() const
{
	TStringBuilder<256> StringBuilder;

	StringBuilder << ALS_GET_TYPE_STRING(FAlsRootMotionSource_Mantling)
		<< TEXTVIEW(" (") << InstanceName << TEXTVIEW(", ") << LocalID << TEXT(')');

	return FString{StringBuilder};
}

void FAlsRootMotionSource_Mantling::AddReferencedObjects(FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(MantlingSettings);
}
