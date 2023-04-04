#include "Notifies/AlsAnimNotify_FootstepEffects.h"

#include "AlsCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsEnumUtility.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsMath.h"
#include "Utility/AlsUtility.h"

// ReSharper disable once CppUnusedIncludeDirective
#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotify_FootstepEffects)

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

	const auto MeshScale{Mesh->GetComponentScale().Z};

	const auto* World{Mesh->GetWorld()};
	const auto* AnimationInstance{Mesh->GetAnimInstance()};

	const auto FootBoneName{FootBone == EAlsFootBone::Left ? UAlsConstants::FootLeftBoneName() : UAlsConstants::FootRightBoneName()};
	const auto FootTransform{Mesh->GetSocketTransform(FootBoneName)};

	const auto FootZAxis{
		FootTransform.TransformVectorNoScale(FootBone == EAlsFootBone::Left
			                                     ? FVector{FootstepEffectsSettings->FootLeftZAxis}
			                                     : FVector{FootstepEffectsSettings->FootRightZAxis})
	};

#if ENABLE_DRAW_DEBUG
	const auto bDisplayDebug{UAlsUtility::ShouldDisplayDebugForActor(Mesh->GetOwner(), UAlsConstants::TracesDebugDisplayName())};
#endif

	FCollisionQueryParams QueryParameters{__FUNCTION__, true, Mesh->GetOwner()};
	QueryParameters.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, FootTransform.GetLocation(),
	                                    FootTransform.GetLocation() - FootZAxis *
	                                    (FootstepEffectsSettings->SurfaceTraceDistance * MeshScale),
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
	}
	else
	{
		Hit.ImpactPoint = FootTransform.GetLocation();
		Hit.ImpactNormal = FVector::UpVector;
	}

	const auto SurfaceType{Hit.PhysMaterial.IsValid() ? Hit.PhysMaterial->SurfaceType.GetValue() : SurfaceType_Default};
	const auto* EffectSettings{FootstepEffectsSettings->Effects.Find(SurfaceType)};

	if (EffectSettings == nullptr)
	{
		for (const auto& Pair : FootstepEffectsSettings->Effects)
		{
			EffectSettings = &Pair.Value;
			break;
		}

		if (EffectSettings == nullptr)
		{
			return;
		}
	}

	const auto FootstepLocation{Hit.ImpactPoint};

	const auto FootstepRotation{
		FRotationMatrix::MakeFromZY(Hit.ImpactNormal,
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
		auto VolumeMultiplier{SoundVolumeMultiplier};

		if (!bIgnoreFootstepSoundBlockCurve && IsValid(AnimationInstance))
		{
			VolumeMultiplier *= 1.0f - UAlsMath::Clamp01(AnimationInstance->GetCurveValue(UAlsConstants::FootstepSoundBlockCurveName()));
		}

		if (FAnimWeight::IsRelevant(VolumeMultiplier) && IsValid(EffectSettings->Sound.LoadSynchronous()))
		{
			UAudioComponent* Audio{nullptr};

			switch (EffectSettings->SoundSpawnMode)
			{
				case EAlsFootstepSoundSpawnMode::SpawnAtTraceHitLocation:
					if (World->WorldType == EWorldType::EditorPreview)
					{
						UGameplayStatics::PlaySoundAtLocation(World, EffectSettings->Sound.Get(), FootstepLocation,
						                                      VolumeMultiplier, SoundPitchMultiplier);
					}
					else
					{
						Audio = UGameplayStatics::SpawnSoundAtLocation(World, EffectSettings->Sound.Get(), FootstepLocation,
						                                               FootstepRotation.Rotator(),
						                                               VolumeMultiplier, SoundPitchMultiplier);
					}
					break;

				case EAlsFootstepSoundSpawnMode::SpawnAttachedToFootBone:
					Audio = UGameplayStatics::SpawnSoundAttached(EffectSettings->Sound.Get(), Mesh, FootBoneName, FVector::ZeroVector,
					                                             FRotator::ZeroRotator, EAttachLocation::SnapToTarget,
					                                             true, VolumeMultiplier, SoundPitchMultiplier);
					break;
			}

			if (IsValid(Audio))
			{
				Audio->SetIntParameter(FName{TEXTVIEW("FootstepType")}, static_cast<int32>(SoundType));
			}
		}
	}

	if (bSpawnDecal && IsValid(EffectSettings->DecalMaterial.LoadSynchronous()))
	{
		const auto DecalRotation{
			FootstepRotation * (FootBone == EAlsFootBone::Left
				                    ? EffectSettings->DecalFootLeftRotationOffset
				                    : EffectSettings->DecalFootRightRotationOffset).Quaternion()
		};

		const auto DecalLocation{
			FootstepLocation + DecalRotation.RotateVector(FVector{EffectSettings->DecalLocationOffset} * MeshScale)
		};

		UDecalComponent* Decal;

		if (EffectSettings->DecalSpawnMode == EAlsFootstepDecalSpawnMode::SpawnAttachedToTraceHitComponent && Hit.Component.IsValid())
		{
			Decal = UGameplayStatics::SpawnDecalAttached(EffectSettings->DecalMaterial.Get(),
			                                             FVector{EffectSettings->DecalSize} * MeshScale,
			                                             Hit.Component.Get(), NAME_None, DecalLocation,
			                                             DecalRotation.Rotator(), EAttachLocation::KeepWorldPosition);
		}
		else
		{
			Decal = UGameplayStatics::SpawnDecalAtLocation(World, EffectSettings->DecalMaterial.Get(),
			                                               FVector{EffectSettings->DecalSize} * MeshScale,
			                                               DecalLocation, DecalRotation.Rotator());
		}

		if (IsValid(Decal))
		{
			Decal->SetFadeOut(EffectSettings->DecalDuration, EffectSettings->DecalFadeOutDuration, false);
		}
	}

	if (bSpawnParticleSystem && IsValid(EffectSettings->ParticleSystem.LoadSynchronous()))
	{
		switch (EffectSettings->ParticleSystemSpawnMode)
		{
			case EAlsFootstepParticleEffectSpawnMode::SpawnAtTraceHitLocation:
			{
				const auto ParticleSystemRotation{
					FootstepRotation * (FootBone == EAlsFootBone::Left
						                    ? EffectSettings->ParticleSystemFootLeftRotationOffset
						                    : EffectSettings->ParticleSystemFootRightRotationOffset).Quaternion()
				};

				const auto ParticleSystemLocation{
					FootstepLocation +
					ParticleSystemRotation.RotateVector(FVector{EffectSettings->ParticleSystemLocationOffset} * MeshScale)
				};

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, EffectSettings->ParticleSystem.Get(),
				                                               ParticleSystemLocation, ParticleSystemRotation.Rotator(),
				                                               FVector::OneVector * MeshScale, true, true, ENCPoolMethod::AutoRelease);
			}
			break;

			case EAlsFootstepParticleEffectSpawnMode::SpawnAttachedToFootBone:
				UNiagaraFunctionLibrary::SpawnSystemAttached(EffectSettings->ParticleSystem.Get(), Mesh, FootBoneName,
				                                             FVector{EffectSettings->ParticleSystemLocationOffset} * MeshScale,
				                                             FootBone == EAlsFootBone::Left
					                                             ? EffectSettings->ParticleSystemFootLeftRotationOffset
					                                             : EffectSettings->ParticleSystemFootRightRotationOffset,
				                                             FVector::OneVector * MeshScale, EAttachLocation::KeepRelativeOffset,
				                                             true, ENCPoolMethod::AutoRelease);
				break;
		}
	}
}
