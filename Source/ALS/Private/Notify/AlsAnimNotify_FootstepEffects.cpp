#include "Notify/AlsAnimNotify_FootstepEffects.h"

#include "AlsAnimationInstance.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsEnumerationUtility.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

FString UAlsAnimNotify_FootstepEffects::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Footstep Effects: {0}"), {GetEnumValueString(FootBone)});
}

void UAlsAnimNotify_FootstepEffects::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComponent, Animation);

	if (!IsValid(MeshComponent) || !IsValid(FootstepEffectsSettings))
	{
		return;
	}

	const auto* AnimationInstance{Cast<UAlsAnimationInstance>(MeshComponent->GetAnimInstance())};
	if (bSkipEffectsWhenInAir && IsValid(AnimationInstance) && AnimationInstance->GetLocomotionMode() == EAlsLocomotionMode::InAir)
	{
		return;
	}

	const auto* World{MeshComponent->GetWorld()};

	const auto FootBoneName{FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftIkBone() : UAlsConstants::FootRightIkBone()};
	const auto FootTransform{MeshComponent->GetSocketTransform(FootBoneName)};

	FVector HitLocation;
	FVector HitNormal;
	TWeakObjectPtr<UPrimitiveComponent> HitComponent;
	TWeakObjectPtr<UPhysicalMaterial> HitPhysicalMaterial;

	const auto* FeetState{
		IsValid(AnimationInstance)
			? (FootBone == EAlsFootBone::Left
				   ? &AnimationInstance->GetFeetState().Left
				   : &AnimationInstance->GetFeetState().Right)
			: nullptr
	};

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebug(MeshComponent->GetOwner(), UAlsConstants::TracesDisplayName())};
#endif

	if (FeetState != nullptr && FeetState->bOffsetHitValid)
	{
		// Reuse hit information from foot offset logic.

		HitLocation = FeetState->OffsetHitLocation;
		HitNormal = FeetState->OffsetHitNormal;
		HitComponent = FeetState->OffsetHitComponent;
		HitPhysicalMaterial = FeetState->OffsetHitPhysicalMaterial;
	}
	else
	{
		FCollisionQueryParams QueryParameters{ANSI_TO_TCHAR(__FUNCTION__), true, MeshComponent->GetOwner()};
		QueryParameters.bReturnPhysicalMaterial = true;

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, FootTransform.GetLocation(),
		                                    FootTransform.GetLocation() - FVector{
			                                    0.0f, 0.0f, FootstepEffectsSettings->SurfaceTraceDistance
		                                    },
		                                    UEngineTypes::ConvertToCollisionChannel(FootstepEffectsSettings->SurfaceTraceChannel),
		                                    QueryParameters))
		{
#if ENABLE_DRAW_DEBUG
			if (bDisplayDebug)
			{
				UAlsUtility::DrawDebugLineTraceSingle(World, Hit.TraceStart, Hit.TraceEnd, Hit.bBlockingHit,
				                                      Hit, {0.333333f, 0.0f, 0.0f}, FLinearColor::Red, 10.0f);
			}
#endif

			HitLocation = Hit.ImpactPoint;
			HitNormal = Hit.ImpactNormal;
			HitComponent = Hit.Component;
			HitPhysicalMaterial = Hit.PhysMaterial;
		}
		else
		{
			HitLocation = FootTransform.GetLocation();
			HitNormal = FVector::UpVector;
			HitComponent = nullptr;
			HitPhysicalMaterial = nullptr;
		}
	}

	const auto SurfaceType{
		HitPhysicalMaterial.IsValid()
			? HitPhysicalMaterial->SurfaceType
			: TEnumAsByte<EPhysicalSurface>(SurfaceType_Default)
	};

	const FAlsFootstepEffectSettings* EffectSettings{nullptr};

	for (const auto& Effect : FootstepEffectsSettings->Effects)
	{
		if (Effect.SurfaceType == SurfaceType)
		{
			EffectSettings = &Effect;
			break;
		}

		if (EffectSettings == nullptr)
		{
			EffectSettings = &Effect;
		}
	}

	if (EffectSettings == nullptr)
	{
		return;
	}

	const auto FootstepLocation{HitLocation};

	const auto FootstepRotation{
		FRotationMatrix::MakeFromZY(HitNormal, FootTransform.TransformVectorNoScale(FootBone == EAlsFootBone::Left
			                                                                            ? FootstepEffectsSettings->FootLeftYAxis
			                                                                            : FootstepEffectsSettings->FootRightYAxis)).
		Rotator()
	};

#if ENABLE_DRAW_DEBUG
	if (bDisplayDebug)
	{
		DrawDebugCoordinateSystem(World, FootstepLocation, FootstepRotation, 25.0f, false, 10.0f, 0, UAlsUtility::DrawLineThickness);
	}
#endif

	if (bSpawnSound)
	{
		auto VolumeMultiplier{SoundVolumeMultiplier};

		if (!bIgnoreFootstepSoundBlockCurve && IsValid(AnimationInstance))
		{
			VolumeMultiplier *= 1.0f - UAlsMath::Clamp01(AnimationInstance->GetCurveValue(UAlsConstants::FootstepSoundBlockCurve()));
		}

		if (VolumeMultiplier > 0.0f && EffectSettings->Sound.LoadSynchronous())
		{
			UAudioComponent* Audio{nullptr};

			switch (EffectSettings->SoundSpawnType)
			{
				case EAlsFootstepSoundSpawnType::SpawnAtTraceHitLocation:
					if (World->WorldType == EWorldType::EditorPreview)
					{
						UGameplayStatics::PlaySoundAtLocation(World, EffectSettings->Sound.Get(), FootstepLocation,
						                                      VolumeMultiplier, SoundPitchMultiplier);
					}
					else
					{
						Audio = UGameplayStatics::SpawnSoundAtLocation(World, EffectSettings->Sound.Get(), FootstepLocation,
						                                               FootstepRotation, VolumeMultiplier, SoundPitchMultiplier);
					}
					break;

				case EAlsFootstepSoundSpawnType::SpawnAttachedToFootBone:
					Audio = UGameplayStatics::SpawnSoundAttached(EffectSettings->Sound.Get(), MeshComponent, FootBoneName,
					                                             FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget,
					                                             true, VolumeMultiplier, SoundPitchMultiplier);
					break;
			}

			if (IsValid(Audio))
			{
				Audio->SetIntParameter(TEXT("FootstepType"), static_cast<int32>(SoundType));
			}
		}
	}

	if (bSpawnDecal && EffectSettings->DecalMaterial.LoadSynchronous())
	{
		const auto DecalRotation{
			FootstepRotation + (FootBone == EAlsFootBone::Left
				                    ? EffectSettings->DecalFootLeftRotationOffset
				                    : EffectSettings->DecalFootRightRotationOffset)
		};

		const auto DecalLocation{FootstepLocation + DecalRotation.RotateVector(EffectSettings->DecalLocationOffset)};

		UDecalComponent* Decal;

		if (EffectSettings->DecalSpawnType == EAlsFootstepDecalSpawnType::SpawnAttachedToTraceHitComponent && HitComponent.IsValid())
		{
			Decal = UGameplayStatics::SpawnDecalAttached(EffectSettings->DecalMaterial.Get(), EffectSettings->DecalSize,
			                                             HitComponent.Get(), NAME_None, DecalLocation,
			                                             DecalRotation, EAttachLocation::KeepWorldPosition);
		}
		else
		{
			Decal = UGameplayStatics::SpawnDecalAtLocation(World, EffectSettings->DecalMaterial.Get(),
			                                               EffectSettings->DecalSize, DecalLocation, DecalRotation);
		}

		if (IsValid(Decal))
		{
			Decal->SetFadeOut(EffectSettings->DecalDuration, EffectSettings->DecalFadeOutDuration, false);
		}
	}

	if (bSpawnParticleSystem && EffectSettings->ParticleSystem.LoadSynchronous())
	{
		switch (EffectSettings->ParticleSystemSpawnType)
		{
			case EAlsFootstepParticleEffectSpawnType::SpawnAtTraceHitLocation:
			{
				const auto ParticleSystemRotation{
					FootstepRotation + (FootBone == EAlsFootBone::Left
						                    ? EffectSettings->ParticleSystemFootLeftRotationOffset
						                    : EffectSettings->ParticleSystemFootLeftRotationOffset)
				};

				const auto ParticleSystemLocation{
					FootstepLocation + ParticleSystemRotation.RotateVector(EffectSettings->ParticleSystemLocationOffset)
				};

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, EffectSettings->ParticleSystem.Get(),
				                                               ParticleSystemLocation, ParticleSystemRotation,
				                                               FVector::OneVector, true, true, ENCPoolMethod::AutoRelease);
			}
			break;

			case EAlsFootstepParticleEffectSpawnType::SpawnAttachedToFootBone:
				UNiagaraFunctionLibrary::SpawnSystemAttached(EffectSettings->ParticleSystem.Get(), MeshComponent, FootBoneName,
				                                             EffectSettings->ParticleSystemLocationOffset,
				                                             EffectSettings->ParticleSystemFootLeftRotationOffset,
				                                             EAttachLocation::KeepRelativeOffset, true, true, ENCPoolMethod::AutoRelease);
				break;
		}
	}
}
