#include "RootMotionSources/AlsRootMotionSource_Mantling.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMantlingSettings.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsMontageUtility.h"
#include "Utility/AlsRotation.h"

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

	const auto* OtherCasted{static_cast<const FAlsRootMotionSource_Mantling*>(Other)}; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

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

	auto BlendInAmount{1.0f};
	const auto& MontageBlendIn{Montage->BlendIn};

	if (MontageBlendIn.GetBlendTime() > UE_SMALL_NUMBER)
	{
		BlendInAmount = FAlphaBlend::AlphaToBlendOption(GetTime() / MontageBlendIn.GetBlendTime(),
		                                                MontageBlendIn.GetBlendOption(),
		                                                MontageBlendIn.GetCustomCurve());
	}

	const auto WarpAmount{UAlsMath::Clamp01(MantlingSettings->MotionWarpingTimeRange.GetRangePct(MontageTime))};

	const auto LocationWarpAmount{
		BlendInAmount * FAlphaBlend::AlphaToBlendOption(WarpAmount,
		                                                MantlingSettings->MotionWarpingLocationBlendOption,
		                                                MantlingSettings->MotionWarpingLocationCustomBlendCurve)
	};

	const auto RotationWarpAmount{
		BlendInAmount * FAlphaBlend::AlphaToBlendOption(WarpAmount,
		                                                MantlingSettings->MotionWarpingRotationBlendOption,
		                                                MantlingSettings->MotionWarpingRotationCustomBlendCurve)
	};

	// Calculate the start and target transforms based on the stored relative transforms to follow moving objects.

	FTransform StartTransform{StartRotation, StartLocation};
	FTransform TargetTransform{TargetRotation, TargetLocation};

	if (MovementBaseUtility::UseRelativeLocation(TargetPrimitive.Get()))
	{
		StartTransform *= TargetPrimitive->GetComponentTransform();
		StartTransform.SetScale3D(FVector::OneVector);

		TargetTransform *= TargetPrimitive->GetComponentTransform();
		TargetTransform.SetScale3D(FVector::OneVector);
	}

	// Interpolate the start and target transform to obtain the warped transform.

	const auto WarpLocation{FMath::Lerp(StartTransform.GetLocation(), TargetTransform.GetLocation(), LocationWarpAmount)};

	auto WarpRotation{FQuat::FastLerp(StartTransform.GetRotation(), TargetTransform.GetRotation(), RotationWarpAmount)};
	WarpRotation.Normalize();

	// Remove the pitch and roll components of the rotation so that the actor's Z axis is always aligned with the gravity direction.

	WarpRotation = UAlsRotation::GetTwist(WarpRotation, -Character.GetGravityDirection());

	const FTransform WarpTransform{WarpRotation, WarpLocation};

	// Extract the current root transform, convert from the mesh space to the actor space.

	auto RootTransform{UAlsMontageUtility::ExtractRootTransformFromMontage(Montage, MontageTime)};
	RootTransform.SetScale3D(FVector::OneVector);

	const FTransform MeshTransform{Character.GetBaseRotationOffset()};
	RootTransform = MeshTransform.GetRelativeTransformReverse(RootTransform);

	// Apply the current root transform to the warped transform to get the target actor transform.

	const auto TargetActorTransform{RootTransform * WarpTransform};

	// Calculate the final root motion by taking the delta between the current actor transform and the target actor transform.

	const FTransform FinalRootMotion{
		TargetActorTransform.GetRotation() * Character.GetActorQuat().Inverse(),
		(TargetActorTransform.GetLocation() - Character.GetActorLocation()) / DeltaTime
	};

	RootMotionParams.Set(FinalRootMotion);
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

	bSuccess &= SerializePackedVector<100, 30>(StartLocation, Archive);

	StartRotation.NetSerialize(Archive, Map, bSuccessLocal);
	StartRotation.Normalize();
	bSuccess &= bSuccessLocal;

	bSuccess &= SerializePackedVector<100, 30>(TargetLocation, Archive);

	TargetRotation.NetSerialize(Archive, Map, bSuccessLocal);
	TargetRotation.Normalize();
	bSuccess &= bSuccessLocal;

	Archive << MontageStartTime;

	return bSuccess;
}

UScriptStruct* FAlsRootMotionSource_Mantling::GetScriptStruct() const
{
	return StaticStruct();
}

FString FAlsRootMotionSource_Mantling::ToSimpleString() const
{
	TStringBuilder<256> StringBuilder{
		InPlace, ALS_GET_TYPE_STRING(FAlsRootMotionSource_Mantling), TEXTVIEW(" ("), InstanceName, TEXTVIEW(", "), LocalID, TEXT(')')
	};

	return FString{StringBuilder};
}

void FAlsRootMotionSource_Mantling::AddReferencedObjects(FReferenceCollector& Collector)
{
	Super::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(MantlingSettings);
}
