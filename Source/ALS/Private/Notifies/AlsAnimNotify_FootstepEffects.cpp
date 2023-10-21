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
#include "Utility/AlsEnumUtility.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotify_FootstepEffects)

#if WITH_EDITOR
void FAlsFootstepEffectSettings::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent)
{
	DecalFootLeftRotationOffsetQuaternion = DecalFootLeftRotationOffset.Quaternion();
	DecalFootRightRotationOffsetQuaternion = DecalFootRightRotationOffset.Quaternion();
	ParticleSystemFootLeftRotationOffsetQuaternion = ParticleSystemFootLeftRotationOffset.Quaternion();
	ParticleSystemFootRightRotationOffsetQuaternion = ParticleSystemFootRightRotationOffset.Quaternion();
}

void UAlsFootstepEffectsSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, DecalSpawnAngleThreshold))
	{
		DecalSpawnAngleThresholdCos = FMath::Cos(FMath::DegreesToRadians(DecalSpawnAngleThreshold));
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, Effects))
	{
		for (auto& Tuple : Effects)
		{
			Tuple.Value.PostEditChangeProperty(PropertyChangedEvent);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FString UAlsAnimNotify_FootstepEffects::GetNotifyName_Implementation() const
{
	TStringBuilder<64> NotifyNameBuilder;

	NotifyNameBuilder << TEXTVIEW("Als Footstep Effects: ") << AlsEnumUtility::GetNameStringByValue(FootBone);

	return FString{NotifyNameBuilder};
}

void UAlsAnimNotify_FootstepEffects::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(Mesh, Animation, EventReference);

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
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebugForActor(Mesh->GetOwner(), UAlsConstants::TracesDebugDisplayName())};
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
		UAlsUtility::DrawDebugLineTraceSingle(World, FootstepHit.TraceStart, FootstepHit.TraceEnd, FootstepHit.bBlockingHit,
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
		                          25.0f, false, 10.0f, 0, UAlsUtility::DrawLineThickness);
	}
#endif

	if (bSpawnSound)
	{
		SpawnSound(Mesh, *EffectSettings, FootstepLocation, FootstepRotation);
	}

	if (bSpawnDecal)
	{
		SpawnDecal(Mesh, *EffectSettings, FootstepLocation, FootstepRotation, FootstepHit, FootZAxis);
	}

	if (bSpawnParticleSystem)
	{
		SpawnParticleSystem(Mesh, *EffectSettings, FootstepLocation, FootstepRotation);
	}
}

void UAlsAnimNotify_FootstepEffects::SpawnSound(USkeletalMeshComponent* Mesh, const FAlsFootstepEffectSettings& EffectSettings,
                                                const FVector& FootstepLocation, const FQuat& FootstepRotation) const
{
	auto VolumeMultiplier{SoundVolumeMultiplier};

	if (!bIgnoreFootstepSoundBlockCurve && IsValid(Mesh->GetAnimInstance()))
	{
		VolumeMultiplier *= 1.0f - UAlsMath::Clamp01(Mesh->GetAnimInstance()->GetCurveValue(UAlsConstants::FootstepSoundBlockCurveName()));
	}

	if (!FAnimWeight::IsRelevant(VolumeMultiplier) || !IsValid(EffectSettings.Sound.LoadSynchronous()))
	{
		return;
	}

	UAudioComponent* Audio{nullptr};

	if (EffectSettings.SoundSpawnMode == EAlsFootstepSoundSpawnMode::SpawnAtTraceHitLocation)
	{
		const auto* World{Mesh->GetWorld()};

		if (World->WorldType == EWorldType::EditorPreview)
		{
			UGameplayStatics::PlaySoundAtLocation(World, EffectSettings.Sound.Get(), FootstepLocation,
			                                      VolumeMultiplier, SoundPitchMultiplier);
		}
		else
		{
			Audio = UGameplayStatics::SpawnSoundAtLocation(World, EffectSettings.Sound.Get(), FootstepLocation,
			                                               FootstepRotation.Rotator(),
			                                               VolumeMultiplier, SoundPitchMultiplier);
		}
	}
	else if (EffectSettings.SoundSpawnMode == EAlsFootstepSoundSpawnMode::SpawnAttachedToFootBone)
	{
		const auto& FootBoneName{
			FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBoneName() : UAlsConstants::FootRightBoneName()
		};

		Audio = UGameplayStatics::SpawnSoundAttached(EffectSettings.Sound.Get(), Mesh, FootBoneName, FVector::ZeroVector,
		                                             FRotator::ZeroRotator, EAttachLocation::SnapToTarget,
		                                             true, VolumeMultiplier, SoundPitchMultiplier);
	}

	if (IsValid(Audio))
	{
		Audio->SetIntParameter(FName{TEXTVIEW("FootstepType")}, static_cast<int32>(SoundType));
	}
}

void UAlsAnimNotify_FootstepEffects::SpawnDecal(USkeletalMeshComponent* Mesh, const FAlsFootstepEffectSettings& EffectSettings,
                                                const FVector& FootstepLocation, const FQuat& FootstepRotation,
                                                const FHitResult& FootstepHit, const FVector& FootZAxis) const
{
	if ((FootstepHit.ImpactNormal | FootZAxis) < FootstepEffectsSettings->DecalSpawnAngleThresholdCos)
	{
		return;
	}

	if (!IsValid(EffectSettings.DecalMaterial.LoadSynchronous()))
	{
		return;
	}

	const auto DecalRotation{
		FootstepRotation * FQuat{
			FootBone == EAlsFootBone::Left
				? EffectSettings.DecalFootLeftRotationOffsetQuaternion
				: EffectSettings.DecalFootRightRotationOffsetQuaternion
		}
	};

	const auto MeshScale{Mesh->GetComponentScale().Z};

	const auto DecalLocation{
		FootstepLocation + DecalRotation.RotateVector(FVector{EffectSettings.DecalLocationOffset} * MeshScale)
	};

	UDecalComponent* Decal{nullptr};

	if (EffectSettings.DecalSpawnMode == EAlsFootstepDecalSpawnMode::SpawnAtTraceHitLocation || !FootstepHit.Component.IsValid())
	{
		Decal = UGameplayStatics::SpawnDecalAtLocation(Mesh->GetWorld(), EffectSettings.DecalMaterial.Get(),
		                                               FVector{EffectSettings.DecalSize} * MeshScale,
		                                               DecalLocation, DecalRotation.Rotator());
	}
	else if (EffectSettings.DecalSpawnMode == EAlsFootstepDecalSpawnMode::SpawnAttachedToTraceHitComponent)
	{
		Decal = UGameplayStatics::SpawnDecalAttached(EffectSettings.DecalMaterial.Get(),
		                                             FVector{EffectSettings.DecalSize} * MeshScale,
		                                             FootstepHit.Component.Get(), NAME_None, DecalLocation,
		                                             DecalRotation.Rotator(), EAttachLocation::KeepWorldPosition);
	}

	if (IsValid(Decal))
	{
		Decal->SetFadeOut(EffectSettings.DecalDuration, EffectSettings.DecalFadeOutDuration, false);
	}
}

void UAlsAnimNotify_FootstepEffects::SpawnParticleSystem(USkeletalMeshComponent* Mesh, const FAlsFootstepEffectSettings& EffectSettings,
                                                         const FVector& FootstepLocation, const FQuat& FootstepRotation) const
{
	if (!IsValid(EffectSettings.ParticleSystem.LoadSynchronous()))
	{
		return;
	}

	const auto MeshScale{Mesh->GetComponentScale().Z};

	if (EffectSettings.ParticleSystemSpawnMode == EAlsFootstepParticleEffectSpawnMode::SpawnAtTraceHitLocation)
	{
		const auto ParticleSystemRotation{
			FootstepRotation * FQuat{
				FootBone == EAlsFootBone::Left
					? EffectSettings.ParticleSystemFootLeftRotationOffsetQuaternion
					: EffectSettings.ParticleSystemFootRightRotationOffsetQuaternion
			}
		};

		const auto ParticleSystemLocation{
			FootstepLocation +
			ParticleSystemRotation.RotateVector(FVector{EffectSettings.ParticleSystemLocationOffset} * MeshScale)
		};

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Mesh->GetWorld(), EffectSettings.ParticleSystem.Get(),
		                                               ParticleSystemLocation, ParticleSystemRotation.Rotator(),
		                                               FVector::OneVector * MeshScale, true, true, ENCPoolMethod::AutoRelease);
	}
	else if (EffectSettings.ParticleSystemSpawnMode == EAlsFootstepParticleEffectSpawnMode::SpawnAttachedToFootBone)
	{
		const auto& FootBoneName{FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBoneName() : UAlsConstants::FootRightBoneName()};

		UNiagaraFunctionLibrary::SpawnSystemAttached(EffectSettings.ParticleSystem.Get(), Mesh, FootBoneName,
		                                             FVector{EffectSettings.ParticleSystemLocationOffset} * MeshScale,
		                                             FRotator{
			                                             FootBone == EAlsFootBone::Left
				                                             ? EffectSettings.ParticleSystemFootLeftRotationOffset
				                                             : EffectSettings.ParticleSystemFootRightRotationOffset
		                                             },
		                                             FVector::OneVector * MeshScale, EAttachLocation::KeepRelativeOffset,
		                                             true, ENCPoolMethod::AutoRelease);
	}
}
