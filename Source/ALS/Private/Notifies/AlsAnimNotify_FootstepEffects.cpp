#include "Notifies/AlsAnimNotify_FootstepEffects.h"

#include "AlsCharacter.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundBase.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsDebugUtility.h"
#include "Utility/AlsEnumUtility.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsMath.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotify_FootstepEffects)

#if WITH_EDITOR
void FAlsFootstepDecalSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	FootLeftRotationOffsetQuaternion = FootLeftRotationOffset.Quaternion();
	FootRightRotationOffsetQuaternion = FootRightRotationOffset.Quaternion();
}

void FAlsFootstepParticleSystemSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	FootLeftRotationOffsetQuaternion = FootLeftRotationOffset.Quaternion();
	FootRightRotationOffsetQuaternion = FootRightRotationOffset.Quaternion();
}

void FAlsFootstepEffectSettings::PostEditChangeProperty(const FPropertyChangedEvent& ChangedEvent)
{
	Decal.PostEditChangeProperty(ChangedEvent);
	ParticleSystem.PostEditChangeProperty(ChangedEvent);
}

void UAlsFootstepEffectsSettings::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(ThisClass, DecalSpawnAngleThreshold))
	{
		DecalSpawnAngleThresholdCos = FMath::Cos(FMath::DegreesToRadians(DecalSpawnAngleThreshold));
	}
	else if (ChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(ThisClass, Effects))
	{
		for (auto& Tuple : Effects)
		{
			Tuple.Value.PostEditChangeProperty(ChangedEvent);
		}
	}

	Super::PostEditChangeProperty(ChangedEvent);
}
#endif

FString UAlsAnimNotify_FootstepEffects::GetNotifyName_Implementation() const
{
	TStringBuilder<64> NotifyNameBuilder{InPlace, TEXTVIEW("Als Footstep Effects: "), AlsEnumUtility::GetNameStringByValue(FootBone)};

	return FString{NotifyNameBuilder};
}

#if WITH_EDITOR
void UAlsAnimNotify_FootstepEffects::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& NotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(NotifyEvent);

	NotifyEvent.bTriggerOnDedicatedServer = false;
}
#endif

void UAlsAnimNotify_FootstepEffects::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
                                            const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::Notify(Mesh, Sequence, NotifyEventReference);

	if (!IsValid(Mesh) || !ALS_ENSURE(IsValid(FootstepEffectsSettings)))
	{
		return;
	}

	const auto* Character{Cast<AAlsCharacter>(Mesh->GetOwner())};

	if (bSkipEffectsWhenInAir && IsValid(Character) && Character->GetLocomotionMode() == AlsLocomotionModeTags::InAir)
	{
		return;
	}

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsDebugUtility::ShouldDisplayDebugForActor(Mesh->GetOwner(), UAlsConstants::TracesDebugDisplayName())};
#endif

	const auto* World{Mesh->GetWorld()};
	const auto MeshScale{Mesh->GetComponentScale().Z};

	const auto& FootBoneName{FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBoneName() : UAlsConstants::FootRightBoneName()};
	const auto FootTransform{Mesh->GetSocketTransform(FootBoneName)};

	const auto FootZAxis{
		FootTransform.TransformVectorNoScale(FootBone == EAlsFootBone::Left
			                                     ? FVector{FootstepEffectsSettings->FootLeftZAxis}
			                                     : FVector{FootstepEffectsSettings->FootRightZAxis})
	};

	FCollisionQueryParams QueryParameters{__FUNCTION__, true, Mesh->GetOwner()};
	QueryParameters.bReturnPhysicalMaterial = true;

	FHitResult FootstepHit;
	if (!World->LineTraceSingleByChannel(FootstepHit, FootTransform.GetLocation(),
	                                     FootTransform.GetLocation() - FootZAxis *
	                                     (FootstepEffectsSettings->SurfaceTraceDistance * MeshScale),
	                                     FootstepEffectsSettings->SurfaceTraceChannel, QueryParameters))
	{
		// As a fallback, trace down the world Z axis if the first trace didn't hit anything.

		World->LineTraceSingleByChannel(FootstepHit, FootTransform.GetLocation(),
		                                FootTransform.GetLocation() - FVector{
			                                0.0f, 0.0f, FootstepEffectsSettings->SurfaceTraceDistance * MeshScale
		                                }, FootstepEffectsSettings->SurfaceTraceChannel, QueryParameters);
	}

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebug)
	{
		UAlsDebugUtility::DrawLineTraceSingle(World, FootstepHit.TraceStart, FootstepHit.TraceEnd, FootstepHit.bBlockingHit,
		                                      FootstepHit, {0.333333f, 0.0f, 0.0f}, FLinearColor::Red, 10.0f);
	}
#endif

	if (!FootstepHit.bBlockingHit)
	{
		return;
	}

	const auto SurfaceType{FootstepHit.PhysMaterial.IsValid() ? FootstepHit.PhysMaterial->SurfaceType.GetValue() : SurfaceType_Default};
	const auto* EffectSettings{FootstepEffectsSettings->Effects.Find(SurfaceType)};

	if (EffectSettings == nullptr)
	{
		for (const auto& Tuple : FootstepEffectsSettings->Effects)
		{
			EffectSettings = &Tuple.Value;
			break;
		}

		if (EffectSettings == nullptr)
		{
			return;
		}
	}

	const auto FootstepLocation{FootstepHit.ImpactPoint};

	const auto FootstepRotation{
		FRotationMatrix::MakeFromZY(FootstepHit.ImpactNormal,
		                            FootTransform.TransformVectorNoScale(FootBone == EAlsFootBone::Left
			                                                                 ? FVector{FootstepEffectsSettings->FootLeftYAxis}
			                                                                 : FVector{FootstepEffectsSettings->FootRightYAxis})).ToQuat()
	};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebug)
	{
		DrawDebugCoordinateSystem(World, FootstepLocation, FootstepRotation.Rotator(),
		                          25.0f, false, 10.0f, 0, UAlsDebugUtility::DrawLineThickness);
	}
#endif

	if (bSpawnSound)
	{
		SpawnSound(Mesh, EffectSettings->Sound, FootstepLocation, FootstepRotation);
	}

	if (bSpawnDecal)
	{
		SpawnDecal(Mesh, EffectSettings->Decal, FootstepLocation, FootstepRotation, FootstepHit, FootZAxis);
	}

	if (bSpawnParticleSystem)
	{
		SpawnParticleSystem(Mesh, EffectSettings->ParticleSystem, FootstepLocation, FootstepRotation);
	}
}

void UAlsAnimNotify_FootstepEffects::SpawnSound(USkeletalMeshComponent* Mesh, const FAlsFootstepSoundSettings& SoundSettings,
                                                const FVector& FootstepLocation, const FQuat& FootstepRotation) const
{
	auto VolumeMultiplier{SoundVolumeMultiplier};

	if (!bIgnoreFootstepSoundBlockCurve && IsValid(Mesh->GetAnimInstance()))
	{
		VolumeMultiplier *= 1.0f - UAlsMath::Clamp01(Mesh->GetAnimInstance()->GetCurveValue(UAlsConstants::FootstepSoundBlockCurveName()));
	}

	if (!FAnimWeight::IsRelevant(VolumeMultiplier) || !IsValid(SoundSettings.Sound.LoadSynchronous()))
	{
		return;
	}

	UAudioComponent* Audio{nullptr};

	if (SoundSettings.SpawnMode == EAlsFootstepSoundSpawnMode::SpawnAtTraceHitLocation)
	{
		const auto* World{Mesh->GetWorld()};

		if (World->WorldType == EWorldType::EditorPreview)
		{
			UGameplayStatics::PlaySoundAtLocation(World, SoundSettings.Sound.Get(), FootstepLocation,
			                                      VolumeMultiplier, SoundPitchMultiplier);
		}
		else
		{
			Audio = UGameplayStatics::SpawnSoundAtLocation(World, SoundSettings.Sound.Get(), FootstepLocation,
			                                               FootstepRotation.Rotator(),
			                                               VolumeMultiplier, SoundPitchMultiplier);
		}
	}
	else if (SoundSettings.SpawnMode == EAlsFootstepSoundSpawnMode::SpawnAttachedToFootBone)
	{
		const auto& FootBoneName{
			FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBoneName() : UAlsConstants::FootRightBoneName()
		};

		Audio = UGameplayStatics::SpawnSoundAttached(SoundSettings.Sound.Get(), Mesh, FootBoneName, FVector::ZeroVector,
		                                             FRotator::ZeroRotator, EAttachLocation::SnapToTarget,
		                                             true, VolumeMultiplier, SoundPitchMultiplier);
	}

	if (IsValid(Audio))
	{
		Audio->SetIntParameter(FName{TEXTVIEW("FootstepType")}, static_cast<int32>(SoundType));
	}
}

void UAlsAnimNotify_FootstepEffects::SpawnDecal(USkeletalMeshComponent* Mesh, const FAlsFootstepDecalSettings& DecalSettings,
                                                const FVector& FootstepLocation, const FQuat& FootstepRotation,
                                                const FHitResult& FootstepHit, const FVector& FootZAxis) const
{
	if ((FootstepHit.ImpactNormal | FootZAxis) < FootstepEffectsSettings->DecalSpawnAngleThresholdCos)
	{
		return;
	}

	if (!IsValid(DecalSettings.DecalMaterial.LoadSynchronous()))
	{
		return;
	}

	const auto DecalRotation{
		FootstepRotation * FQuat{
			FootBone == EAlsFootBone::Left
				? DecalSettings.FootLeftRotationOffsetQuaternion
				: DecalSettings.FootRightRotationOffsetQuaternion
		}
	};

	const auto MeshScale{Mesh->GetComponentScale().Z};

	const auto DecalLocation{
		FootstepLocation + DecalRotation.RotateVector(FVector{DecalSettings.LocationOffset} * MeshScale)
	};

	UDecalComponent* Decal{nullptr};

	if (DecalSettings.SpawnMode == EAlsFootstepDecalSpawnMode::SpawnAtTraceHitLocation || !FootstepHit.Component.IsValid())
	{
		Decal = UGameplayStatics::SpawnDecalAtLocation(Mesh->GetWorld(), DecalSettings.DecalMaterial.Get(),
		                                               FVector{DecalSettings.Size} * MeshScale,
		                                               DecalLocation, DecalRotation.Rotator());
	}
	else if (DecalSettings.SpawnMode == EAlsFootstepDecalSpawnMode::SpawnAttachedToTraceHitComponent)
	{
		Decal = UGameplayStatics::SpawnDecalAttached(DecalSettings.DecalMaterial.Get(),
		                                             FVector{DecalSettings.Size} * MeshScale,
		                                             FootstepHit.Component.Get(), NAME_None, DecalLocation,
		                                             DecalRotation.Rotator(), EAttachLocation::KeepWorldPosition);
	}

	if (IsValid(Decal))
	{
		Decal->SetFadeOut(DecalSettings.Duration, DecalSettings.FadeOutDuration, false);
	}
}

void UAlsAnimNotify_FootstepEffects::SpawnParticleSystem(USkeletalMeshComponent* Mesh,
                                                         const FAlsFootstepParticleSystemSettings& ParticleSystemSettings,
                                                         const FVector& FootstepLocation, const FQuat& FootstepRotation) const
{
	if (!IsValid(ParticleSystemSettings.ParticleSystem.LoadSynchronous()))
	{
		return;
	}

	const auto MeshScale{Mesh->GetComponentScale().Z};

	if (ParticleSystemSettings.SpawnMode == EAlsFootstepParticleEffectSpawnMode::SpawnAtTraceHitLocation)
	{
		const auto ParticleSystemRotation{
			FootstepRotation * FQuat{
				FootBone == EAlsFootBone::Left
					? ParticleSystemSettings.FootLeftRotationOffsetQuaternion
					: ParticleSystemSettings.FootRightRotationOffsetQuaternion
			}
		};

		const auto ParticleSystemLocation{
			FootstepLocation +
			ParticleSystemRotation.RotateVector(FVector{ParticleSystemSettings.LocationOffset} * MeshScale)
		};

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Mesh->GetWorld(), ParticleSystemSettings.ParticleSystem.Get(),
		                                               ParticleSystemLocation, ParticleSystemRotation.Rotator(),
		                                               FVector::OneVector * MeshScale, true, true, ENCPoolMethod::AutoRelease);
	}
	else if (ParticleSystemSettings.SpawnMode == EAlsFootstepParticleEffectSpawnMode::SpawnAttachedToFootBone)
	{
		const auto& FootBoneName{FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBoneName() : UAlsConstants::FootRightBoneName()};

		UNiagaraFunctionLibrary::SpawnSystemAttached(ParticleSystemSettings.ParticleSystem.Get(), Mesh, FootBoneName,
		                                             FVector{ParticleSystemSettings.LocationOffset} * MeshScale,
		                                             FRotator{
			                                             FootBone == EAlsFootBone::Left
				                                             ? ParticleSystemSettings.FootLeftRotationOffset
				                                             : ParticleSystemSettings.FootRightRotationOffset
		                                             },
		                                             FVector::OneVector * MeshScale, EAttachLocation::KeepRelativeOffset,
		                                             true, ENCPoolMethod::AutoRelease);
	}
}
