#pragma once

#include "GameFramework/RootMotionSource.h"
#include "AlsRootMotionSource_Mantling.generated.h"

class UAlsMantlingSettings;

USTRUCT()
struct ALS_API FAlsRootMotionSource_Mantling : public FRootMotionSource
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<const UAlsMantlingSettings> MantlingSettings{nullptr};

	UPROPERTY()
	TWeakObjectPtr<const UPrimitiveComponent> TargetPrimitive;

	UPROPERTY()
	FVector TargetRelativeLocation{ForceInit};

	UPROPERTY()
	FRotator TargetRelativeRotation{ForceInit};

	UPROPERTY()
	FVector ActorFeetLocationOffset{ForceInit};

	UPROPERTY()
	FRotator ActorRotationOffset{ForceInit};

	UPROPERTY()
	FVector TargetAnimationLocation{ForceInit};

	UPROPERTY(Meta = (ClampMin = 0, ForceUnits = "s"))
	float MontageStartTime{0.0f};

public:
	FAlsRootMotionSource_Mantling();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual void PrepareRootMotion(float SimulationDeltaTime, float DeltaTime, const ACharacter& Character,
	                               const UCharacterMovementComponent& Movement) override;

	virtual bool NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bSuccess) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual FString ToSimpleString() const override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

template <>
struct TStructOpsTypeTraits<FAlsRootMotionSource_Mantling> : public TStructOpsTypeTraitsBase2<FAlsRootMotionSource_Mantling>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
