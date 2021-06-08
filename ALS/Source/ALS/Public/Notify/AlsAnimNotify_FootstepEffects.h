#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"

#include "AlsAnimNotify_FootstepEffects.generated.h"

class USoundBase;
class UMaterialInterface;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class EAlsFootBone : uint8
{
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EAlsFootstepSoundType : uint8
{
	Step,
	WalkRun,
	Land
};

UENUM(BlueprintType)
enum class EAlsFootstepSoundSpawnType : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToFootBone
};

UENUM(BlueprintType)
enum class EAlsFootstepDecalSpawnType : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToTraceHitComponent
};

UENUM(BlueprintType)
enum class EAlsFootstepParticleEffectSpawnType : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToFootBone
};

USTRUCT(BlueprintType)
struct ALS_API FAlsFootstepEffectSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	EAlsFootstepSoundSpawnType SoundSpawnType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound",
		Meta = (EditCondition = "SoundSpawnType == EAlsFootstepSoundSpawnType::SpawnAttachedToFootBone"))
	TEnumAsByte<EAttachLocation::Type> SoundAttachLocationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FVector SoundLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FRotator SoundRotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	TSoftObjectPtr<UMaterialInterface> DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	EAlsFootstepDecalSpawnType DecalSpawnType{EAlsFootstepDecalSpawnType::SpawnAttachedToTraceHitComponent};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal",
		Meta = (EditCondition = "DecalSpawnType == EAlsFootstepDecalSpawnType::SpawnAttachedToTraceHitComponent"))
	TEnumAsByte<EAttachLocation::Type> DecalAttachLocationType{EAttachLocation::KeepWorldPosition};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	float DecalDuration{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	FVector DecalLocationOffset{8.0f, 0.0f, 1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	FRotator DecalRotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	FVector DecalSize{10.0f, 20.0f, 20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	TSoftObjectPtr<UNiagaraSystem> ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	EAlsFootstepParticleEffectSpawnType ParticleSystemSpawnType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System",
		Meta = (EditCondition = "ParticleSystemSpawnType == EAlsFootstepParticleEffectSpawnType::SpawnAttachedToFootBone"))
	TEnumAsByte<EAttachLocation::Type> ParticleSystemAttachLocationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	FVector ParticleSystemLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	FRotator ParticleSystemRotationOffset;
};

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsFootstepEffectsSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> SurfaceTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SurfaceTraceDistance{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (TitleProperty = "SurfaceType"))
	TArray<FAlsFootstepEffectSettings> Effects;
};

UCLASS(Meta = (DisplayName = "Als Footstep Effects Animation Notify"))
class ALS_API UAlsAnimNotify_FootstepEffects : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	UAlsFootstepEffectsSettings* FootstepEffectsSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	EAlsFootBone FootBone{EAlsFootBone::Right};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (AllowPrivateAccess))
	bool bSpawnSound{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (AllowPrivateAccess))
	float SoundVolumeMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (AllowPrivateAccess))
	float SoundPitchMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (AllowPrivateAccess))
	EAlsFootstepSoundType SoundType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (AllowPrivateAccess))
	bool bIgnoreFootstepSoundBlockCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Decal", Meta = (AllowPrivateAccess))
	bool bSpawnDecal{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Decal", Meta = (AllowPrivateAccess, EditCondition = "bSpawnDecal"))
	bool bMirrorDecalX = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Decal", Meta = (AllowPrivateAccess, EditCondition = "bSpawnDecal"))
	bool bMirrorDecalY = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Decal", Meta = (AllowPrivateAccess, EditCondition = "bSpawnDecal"))
	bool bMirrorDecalZ = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Particle System", Meta = (AllowPrivateAccess))
	bool bSpawnParticleSystem{true};

public:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
