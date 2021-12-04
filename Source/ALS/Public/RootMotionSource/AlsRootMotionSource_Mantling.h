#pragma once

#include "GameFramework/RootMotionSource.h"

#include "AlsRootMotionSource_Mantling.generated.h"

struct FAlsMantlingParameters;
class UAlsMantlingSettings;

USTRUCT()
struct ALS_API FAlsRootMotionSource_Mantling : public FRootMotionSource
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UAlsMantlingSettings* MantlingSettings{nullptr};

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UPrimitiveComponent> TargetPrimitive;

	UPROPERTY(EditAnywhere)
	FVector_NetQuantize10 TargetRelativeLocation{ForceInit};

	UPROPERTY(EditAnywhere)
	FRotator TargetRelativeRotation{ForceInit};

	UPROPERTY(EditAnywhere)
	FVector_NetQuantize10 ActorLocationOffset{ForceInit};

	UPROPERTY(EditAnywhere)
	FRotator ActorRotationOffset{ForceInit};

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float MantlingHeight{0.0f};

public:
	FAlsRootMotionSource_Mantling();

	virtual FRootMotionSource* Clone() const override;

	virtual bool Matches(const FRootMotionSource* Other) const override;

	virtual void PrepareRootMotion(float SimulationDeltaTime, float DeltaTime, const ACharacter& Character,
	                               const UCharacterMovementComponent& Movement) override;

	virtual bool NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bSuccess) override;

	virtual UScriptStruct* GetScriptStruct() const override;

	virtual FString ToSimpleString() const override;

	virtual void AddReferencedObjects(class FReferenceCollector& Collector) override;
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
