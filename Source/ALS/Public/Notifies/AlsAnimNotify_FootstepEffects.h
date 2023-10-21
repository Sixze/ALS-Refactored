#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "AlsAnimNotify_FootstepEffects.generated.h"

enum EPhysicalSurface : int;
struct FHitResult;
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
enum class EAlsFootstepSoundSpawnMode : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToFootBone
};

UENUM(BlueprintType)
enum class EAlsFootstepDecalSpawnMode : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToTraceHitComponent
};

UENUM(BlueprintType)
enum class EAlsFootstepParticleEffectSpawnMode : uint8
{
	SpawnAtTraceHitLocation,
	SpawnAttachedToFootBone
};

USTRUCT(BlueprintType)
struct ALS_API FAlsFootstepEffectSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	EAlsFootstepSoundSpawnMode SoundSpawnMode{EAlsFootstepSoundSpawnMode::SpawnAtTraceHitLocation};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TSoftObjectPtr<UMaterialInterface> DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (AllowPreserveRatio))
	FVector3f DecalSize{10.0f, 20.0f, 20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float DecalDuration{4.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "s"))
	float DecalFadeOutDuration{2.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	EAlsFootstepDecalSpawnMode DecalSpawnMode{EAlsFootstepDecalSpawnMode::SpawnAttachedToTraceHitComponent};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f DecalLocationOffset{0.0f, -10.0f, -1.75f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FRotator3f DecalFootLeftRotationOffset{90.0f, 0.0f, 90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FRotator3f DecalFootRightRotationOffset{-90.0f, 0.0f, -90.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FQuat4f DecalFootLeftRotationOffsetQuaternion{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FQuat4f DecalFootRightRotationOffsetQuaternion{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TSoftObjectPtr<UNiagaraSystem> ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	EAlsFootstepParticleEffectSpawnMode ParticleSystemSpawnMode{EAlsFootstepParticleEffectSpawnMode::SpawnAtTraceHitLocation};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f ParticleSystemLocationOffset{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FRotator3f ParticleSystemFootLeftRotationOffset{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FRotator3f ParticleSystemFootRightRotationOffset{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FQuat4f ParticleSystemFootLeftRotationOffsetQuaternion{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FQuat4f ParticleSystemFootRightRotationOffsetQuaternion{ForceInit};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
#endif
};

UCLASS(Blueprintable, BlueprintType)
class ALS_API UAlsFootstepEffectsSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TEnumAsByte<ECollisionChannel> SurfaceTraceChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float SurfaceTraceDistance{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Left Y Axis")
	FVector3f FootLeftYAxis{0.0f, 0.0f, 1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Left Z Axis")
	FVector3f FootLeftZAxis{1.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Right Y Axis")
	FVector3f FootRightYAxis{0.0f, 0.0f, 1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", DisplayName = "Foot Right Z Axis")
	FVector3f FootRightZAxis{-1.0f, 0.0f, 0.0f};

	// Prevents footstep decals from spawning if the angle between the foot's Z axis and the surface normal exceeds this value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 90, ForceUnits = "deg"))
	float DecalSpawnAngleThreshold{35.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Settings", AdvancedDisplay, Meta = (ClampMin = 0, ClampMax = 1))
	float DecalSpawnAngleThresholdCos{FMath::Cos(FMath::DegreesToRadians(35.0f))};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ForceInlineRow))
	TMap<TEnumAsByte<EPhysicalSurface>, FAlsFootstepEffectSettings> Effects;

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

UCLASS(DisplayName = "Als Footstep Effects Animation Notify",
	AutoExpandCategories = ("Settings|Sound", "Settings|Decal", "Settings|Particle System"))
class ALS_API UAlsAnimNotify_FootstepEffects : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UAlsFootstepEffectsSettings> FootstepEffectsSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EAlsFootBone FootBone{EAlsFootBone::Left};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	bool bSkipEffectsWhenInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	bool bSpawnSound{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (ClampMin = 0, ForceUnits = "x"))
	float SoundVolumeMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound", Meta = (ClampMin = 0, ForceUnits = "x"))
	float SoundPitchMultiplier{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	EAlsFootstepSoundType SoundType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
	bool bIgnoreFootstepSoundBlockCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Decal")
	bool bSpawnDecal{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Particle System")
	bool bSpawnParticleSystem{true};

public:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;

private:
	void SpawnSound(USkeletalMeshComponent* Mesh, const FAlsFootstepEffectSettings& EffectSettings,
	                const FVector& FootstepLocation, const FQuat& FootstepRotation) const;

	void SpawnDecal(USkeletalMeshComponent* Mesh, const FAlsFootstepEffectSettings& EffectSettings,
	                const FVector& FootstepLocation, const FQuat& FootstepRotation,
	                const FHitResult& FootstepHit, const FVector& FootZAxis) const;

	void SpawnParticleSystem(USkeletalMeshComponent* Mesh, const FAlsFootstepEffectSettings& EffectSettings,
	                         const FVector& FootstepLocation, const FQuat& FootstepRotation) const;
};
