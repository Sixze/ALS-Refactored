#pragma once

#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "Engine/Scene.h"
#include "Utility/AlsConstants.h"

#include "AlsCameraSettings.generated.h"

USTRUCT(BlueprintType)
struct ALSCAMERA_API FAlsFirstPersonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 5, ClampMax = 360))
	float Fov{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CameraSocket{TEXT("FirstPersonCamera")};
};

USTRUCT(BlueprintType)
struct ALSCAMERA_API FAlsThirdPersonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 5, ClampMax = 360))
	float Fov{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FirstPivotSocket{UAlsConstants::RootBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SecondPivotSocket{UAlsConstants::HeadBone()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float TraceRadius{15.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> TraceChannel{UEngineTypes::ConvertToTraceType(ECC_Visibility)};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LeftTraceShoulderSocket{TEXT("ThirdPersonTraceShoulderLeft")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RightTraceShoulderSocket{TEXT("ThirdPersonTraceShoulderRight")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceOverrideOffset{0.0f, 0.0f, 40.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowTraceDistanceSmoothing{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "bAllowTraceDistanceSmoothing"))
	float TraceDistanceInterpolationSpeed{3.0f};
};

USTRUCT(BlueprintType)
struct ALSCAMERA_API FAlsCameraLagSubsteppingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseLagSubstepping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "bUseCameraLagSubstepping", ClampMin = 0.005, ClampMax = 0.5))
	float LagSubstepDeltaTime{1.0f / 60.0f};
};

UCLASS(Blueprintable, BlueprintType)
class ALSCAMERA_API UAlsCameraSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsFirstPersonCameraSettings FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsThirdPersonCameraSettings ThirdPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsCameraLagSubsteppingSettings CameraLagSubstepping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	FPostProcessSettings PostProcess;

public:
#if WITH_EDITORONLY_DATA
	virtual void Serialize(FStructuredArchive::FRecord Record) override;
#endif
};
