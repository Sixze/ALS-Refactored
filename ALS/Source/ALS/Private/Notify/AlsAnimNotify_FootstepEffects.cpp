#include "Notify/AlsAnimNotify_FootstepEffects.h"

#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

FString UAlsAnimNotify_FootstepEffects::GetNotifyName_Implementation() const
{
	return TEXT("Als Footstep Effects");
}

void UAlsAnimNotify_FootstepEffects::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComponent, Animation);

	if (!IsValid(MeshComponent) || !IsValid(FootstepEffectsSettings))
	{
		return;
	}

	auto* Owner{MeshComponent->GetOwner()};
	if (!IsValid(Owner))
	{
		return;
	}

	auto* World{Owner->GetWorld()};

	const auto FootBoneName{FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBone() : UAlsConstants::FootRightBone()};
	const auto FootTransform{MeshComponent->GetSocketTransform(FootBoneName)};

	FHitResult Hit;
	World->LineTraceSingleByChannel(Hit, FootTransform.GetLocation(),
	                                FootTransform.GetLocation() - FVector{0.0f, 0.0f, FootstepEffectsSettings->SurfaceTraceDistance},
	                                UEngineTypes::ConvertToCollisionChannel(FootstepEffectsSettings->SurfaceTraceChannel),
	                                {__FUNCTION__, true, Owner});

#if ENABLE_DRAW_DEBUG
	if (UAlsUtility::ShouldDisplayDebug(Owner, UAlsConstants::TracesDisplayName()))
	{
		UAlsUtility::DrawDebugLineTraceSingle(World, Hit.TraceStart, Hit.TraceEnd, Hit.bBlockingHit,
		                                      Hit, {0.333333f, 0.0f, 0.0f}, FLinearColor::Red, 10.0f);
	}
#endif

	const FAlsFootstepEffectSettings* EffectSettings{nullptr};
	const EPhysicalSurface SurfaceType{Hit.PhysMaterial.IsValid() ? Hit.PhysMaterial->SurfaceType : SurfaceType_Default};

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

	if (bSpawnSound)
	{
		auto ResultVolumeMultiplier{SoundVolumeMultiplier};

		if (!bIgnoreFootstepSoundBlockCurve)
		{
			auto* AnimationInstance{(MeshComponent->GetAnimInstance())};
			if (IsValid(AnimationInstance))
			{
				ResultVolumeMultiplier *=
					1.0f - UAlsMath::Clamp01(AnimationInstance->GetCurveValue(UAlsConstants::FootstepSoundBlockCurve()));
			}
		}

		if (ResultVolumeMultiplier > 0.0f && EffectSettings->Sound.LoadSynchronous())
		{
			UAudioComponent* Audio{nullptr};

			switch (EffectSettings->SoundSpawnType)
			{
				case EAlsFootstepSoundSpawnType::SpawnAtTraceHitLocation:
					Audio = UGameplayStatics::SpawnSoundAtLocation(World, EffectSettings->Sound.Get(),
					                                               (Hit.bBlockingHit ? Hit.Location : FootTransform.GetLocation()) +
					                                               Owner->GetTransform().TransformVector(
						                                               EffectSettings->SoundLocationOffset),
					                                               FootTransform.Rotator() + EffectSettings->SoundRotationOffset,
					                                               ResultVolumeMultiplier, SoundPitchMultiplier);
					break;

				case EAlsFootstepSoundSpawnType::SpawnAttachedToFootBone:
					Audio = UGameplayStatics::SpawnSoundAttached(EffectSettings->Sound.Get(), MeshComponent, FootBoneName,
					                                             EffectSettings->SoundLocationOffset, EffectSettings->SoundRotationOffset,
					                                             EffectSettings->SoundAttachLocationType, true,
					                                             ResultVolumeMultiplier, SoundPitchMultiplier);
					break;
			}

			if (IsValid(Audio))
			{
				Audio->SetIntParameter(TEXT("FootstepType"), static_cast<int32>(SoundType));
			}
		}
	}

	if (!Hit.bBlockingHit)
	{
		return;
	}

	if (bSpawnDecal && EffectSettings->DecalMaterial.LoadSynchronous())
	{
		const auto DecalLocation{Hit.Location + Owner->GetTransform().TransformVector(EffectSettings->DecalLocationOffset)};

		const FVector DecalSize{
			bMirrorDecalX ? -EffectSettings->DecalSize.X : EffectSettings->DecalSize.X,
			bMirrorDecalY ? -EffectSettings->DecalSize.Y : EffectSettings->DecalSize.Y,
			bMirrorDecalZ ? -EffectSettings->DecalSize.Z : EffectSettings->DecalSize.Z
		};

		switch (EffectSettings->DecalSpawnType)
		{
			case EAlsFootstepDecalSpawnType::SpawnAtTraceHitLocation:
				UGameplayStatics::SpawnDecalAtLocation(World, EffectSettings->DecalMaterial.Get(), DecalSize, DecalLocation,
				                                       FootTransform.Rotator() + EffectSettings->DecalRotationOffset,
				                                       EffectSettings->DecalDuration);
				break;

			case EAlsFootstepDecalSpawnType::SpawnAttachedToTraceHitComponent:
				UGameplayStatics::SpawnDecalAttached(EffectSettings->DecalMaterial.Get(), DecalSize, Hit.Component.Get(), NAME_None,
				                                     DecalLocation, FootTransform.Rotator() + EffectSettings->DecalRotationOffset,
				                                     EffectSettings->DecalAttachLocationType, EffectSettings->DecalDuration);
				break;
		}
	}

	if (bSpawnParticleSystem && EffectSettings->ParticleSystem.LoadSynchronous())
	{
		switch (EffectSettings->ParticleSystemSpawnType)
		{
			case EAlsFootstepParticleEffectSpawnType::SpawnAtTraceHitLocation:
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, EffectSettings->ParticleSystem.Get(), Hit.Location +
					                                               Owner->GetTransform().TransformVector(
						                                               EffectSettings->ParticleSystemLocationOffset),
				                                               FootTransform.Rotator() + EffectSettings->ParticleSystemRotationOffset,
				                                               FVector::OneVector, true, true, ENCPoolMethod::AutoRelease);
				break;

			case EAlsFootstepParticleEffectSpawnType::SpawnAttachedToFootBone:
				UNiagaraFunctionLibrary::SpawnSystemAttached(EffectSettings->ParticleSystem.Get(), MeshComponent, FootBoneName,
				                                             EffectSettings->ParticleSystemLocationOffset,
				                                             EffectSettings->ParticleSystemRotationOffset,
				                                             EffectSettings->ParticleSystemAttachLocationType,
				                                             true, true, ENCPoolMethod::AutoRelease);
				break;
		}
	}
}
