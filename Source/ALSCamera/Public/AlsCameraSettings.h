#pragma once

#include "Engine/DataAsset.h"
#include "Engine/Scene.h"
#include "Utility/AlsConstants.h"
#include "AlsCameraSettings.generated.h"

USTRUCT(BlueprintType)
struct ALSCAMERA_API FAlsFirstPersonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 5, ClampMax = 175, ForceUnits = "deg"))
	float FieldOfView{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName CameraSocketName{TEXTVIEW("FirstPersonCamera")};
};

USTRUCT(BlueprintType)
struct ALSCAMERA_API FAlsTraceDistanceSmoothingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0))
	float InterpolationSpeed{3.0f};
};

USTRUCT(BlueprintType)
struct ALSCAMERA_API FAlsThirdPersonCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 5, ClampMax = 175, ForceUnits = "deg"))
	float FieldOfView{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName FirstPivotSocketName{UAlsConstants::RootBoneName()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName SecondPivotSocketName{UAlsConstants::HeadBoneName()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TraceRadius{15.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TEnumAsByte<ECollisionChannel> TraceChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName TraceShoulderLeftSocketName{TEXTVIEW("ThirdPersonTraceShoulderLeft")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FName TraceShoulderRightSocketName{TEXTVIEW("ThirdPersonTraceShoulderRight")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f TraceOverrideOffset{0.0f, 0.0f, 40.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (InlineEditConditionToggle))
	uint8 bEnableTraceDistanceSmoothing : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		DisplayName = "Enable Trace Distance Smoothing", Meta = (EditCondition = "bEnableTraceDistanceSmoothing"))
	FAlsTraceDistanceSmoothingSettings TraceDistanceSmoothing;
};

UCLASS(Blueprintable, BlueprintType)
class ALSCAMERA_API UAlsCameraSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	uint8 bIgnoreTimeDilation : 1 {true};

	// Camera will be teleported if the actor has moved farther than this
	// distance in 1 frame. If zero is specified, teleportation will be disabled.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float TeleportDistanceThreshold{200.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsFirstPersonCameraSettings FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FAlsThirdPersonCameraSettings ThirdPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FPostProcessSettings PostProcess;

public:
#if WITH_EDITORONLY_DATA
	virtual void Serialize(FArchive& Archive) override;
#endif
};
