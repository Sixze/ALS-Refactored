#include "RootMotionSources/AlsRootMotionSource_Mantling.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMantlingSettings.h"
#include "Utility/AlsMacros.h"

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

	const auto MontageTime{MontageStartTime + GetTime() * MontagePlayRate};

	// Synchronize the mantling animation montage's time with the mantling root motion source's time.
	// Delta time subtraction is necessary here, otherwise there will be a one frame lag between them.

	Character.GetMesh()->GetAnimInstance()->Montage_SetPosition(MantlingSettings->Montage, FMath::Max(0.0f, MontageTime - DeltaTime));

	// Calculate the target transform based on the stored relative transform to follow moving objects.

	auto TargetTransform{
		MovementBaseUtility::UseRelativeLocation(TargetPrimitive.Get())
			? FTransform{TargetRelativeRotation, TargetRelativeLocation, TargetPrimitive->GetComponentScale()}
			.GetRelativeTransformReverse(TargetPrimitive->GetComponentTransform())
			: FTransform{TargetRelativeRotation, TargetRelativeLocation}
	};

	FVector LocationOffset;
	FRotator RotationOffset;

	auto BlendInAmount{1.0f};

	if (MantlingSettings->bUseMontageBlendIn)
	{
		const auto& MontageBlendIn{MantlingSettings->Montage->BlendIn};
		if (MontageBlendIn.GetBlendTime() > 0.0f)
		{
			BlendInAmount = FAlphaBlend::AlphaToBlendOption(GetTime() / MontageBlendIn.GetBlendTime(),
			                                                MontageBlendIn.GetBlendOption(), MontageBlendIn.GetCustomCurve());
		}
	}
	else
	{
		if (IsValid(MantlingSettings->BlendInCurve))
		{
			BlendInAmount = MantlingSettings->BlendInCurve->GetFloatValue(GetTime());
		}
	}

	if (!FAnimWeight::IsRelevant(BlendInAmount))
	{
		LocationOffset = ActorFeetLocationOffset;
		RotationOffset = ActorRotationOffset;
	}
	else
	{
		const FVector3f InterpolationAndCorrectionAmounts{
			MantlingSettings->InterpolationAndCorrectionAmountsCurve->GetVectorValue(MontageTime)
		};

		const auto InterpolationAmount{InterpolationAndCorrectionAmounts.X};
		const auto HorizontalCorrectionAmount{InterpolationAndCorrectionAmounts.Y};
		const auto VerticalCorrectionAmount{InterpolationAndCorrectionAmounts.Z};

		if (!FAnimWeight::IsRelevant(InterpolationAmount))
		{
			LocationOffset = FVector::ZeroVector;
			RotationOffset = FRotator::ZeroRotator;
		}
		else
		{
			// Calculate the animation offset. This is the location at which the actual animation starts relative to the target transform.

			auto AnimationLocationOffset{TargetTransform.GetUnitAxis(EAxis::X) * MantlingSettings->StartRelativeLocation.X};
			AnimationLocationOffset.Z = MantlingSettings->StartRelativeLocation.Z;
			AnimationLocationOffset *= Character.GetMesh()->GetComponentScale().Z;

			// Blend into the animation offset and the final offset at the same time.
			// Horizontal and vertical blends use different correction amounts.

			LocationOffset.X = FMath::Lerp(ActorFeetLocationOffset.X, AnimationLocationOffset.X, HorizontalCorrectionAmount) *
			                   InterpolationAmount;
			LocationOffset.Y = FMath::Lerp(ActorFeetLocationOffset.Y, AnimationLocationOffset.Y, HorizontalCorrectionAmount) *
			                   InterpolationAmount;
			LocationOffset.Z = FMath::Lerp(ActorFeetLocationOffset.Z, AnimationLocationOffset.Z, VerticalCorrectionAmount) *
			                   InterpolationAmount;

			// The actor's rotation offset must be normalized for this code block to work properly.

			RotationOffset = ActorRotationOffset * (1.0f - HorizontalCorrectionAmount) * InterpolationAmount;
		}

		// Initial blend in allows the actor to blend into the interpolation and correction curves at
		// the midpoint. This prevents pops when mantling an object lower than the animated mantling.

		if (!FAnimWeight::IsFullWeight(BlendInAmount))
		{
			LocationOffset = FMath::Lerp(ActorFeetLocationOffset, LocationOffset, BlendInAmount);
			RotationOffset = FMath::Lerp(ActorRotationOffset, RotationOffset, BlendInAmount);
		}
	}

	// Apply final offsets.

	TargetTransform.AddToTranslation(LocationOffset);
	TargetTransform.ConcatenateRotation(RotationOffset.Quaternion());

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

	Archive << MontageStartTime;
	Archive << MontagePlayRate;

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
