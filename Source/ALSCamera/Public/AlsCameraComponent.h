#pragma once

#include "Camera/CameraTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMath.h"

#include "AlsCameraComponent.generated.h"

class AAlsCharacter;

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

UCLASS()
class ALSCAMERA_API UAlsCameraComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsFirstPersonCameraSettings FirstPersonSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FAlsThirdPersonCameraSettings ThirdPersonSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess, ClampMin = 0, ClampMax = 1))
	float PostProcessWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	FPostProcessSettings PostProcessSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay, Meta = (AllowPrivateAccess))
	bool bUseLagSubstepping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", AdvancedDisplay,
		Meta = (AllowPrivateAccess, EditCondition = "bUseCameraLagSubstepping", ClampMin = 0.005, ClampMax = 0.5))
	float LagSubstepDeltaTime{1.0f / 60.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	AAlsCharacter* AlsCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FVector PivotTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FVector PivotLagLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FVector PivotLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FVector CameraLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FRotator CameraRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess, ClampMin = 0, ClampMax = 1))
	float TraceDistanceRatio{1.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess, ClampMin = 5, ClampMax = 360))
	float CameraFov;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bRightShoulder{true};

public:
	UAlsCameraComponent();

#if WITH_EDITORONLY_DATA
	virtual void Serialize(FArchive& Archive) override;
#endif

	virtual void OnRegister() override;

	virtual void Activate(bool bReset) override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetPostProcessWeight() const;

	void SetPostProcessWeight(bool NewPostProcessWeight);

	FPostProcessSettings& GetPostProcessSettingsMutable();

	bool IsRightShoulder() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Camera")
	void SetRightShoulder(bool bNewRightShoulder);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera")
	FVector GetFirstPersonCameraLocation() const;

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera")
	FTransform GetThirdPersonPivotTransform() const;

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera")
	FVector GetThirdPersonTraceStartLocation() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Camera")
	void GetViewInfo(FMinimalViewInfo& ViewInfo) const;

private:
	void TickCamera(float DeltaTime, bool bAllowLag = true);

	FRotator CalculateCameraRotation(const FRotator& CameraTargetRotation, float DeltaTime, bool bAllowLag) const;

	FVector CalculatePivotLagLocation(const FQuat& CameraYawRotation, float DeltaTime, bool bAllowLag) const;

	FVector CalculatePivotOffset(const FQuat& PivotTargetRotation) const;

	FVector CalculateCameraOffset() const;

	FVector CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
	                             float DeltaTime, bool bAllowLag, float& NewTraceDistanceRatio) const;

	bool TryFindBlockingGeometryAdjustedLocation(FVector& Location, bool bDisplayDebugCameraTraces) const;

	// Debug

public:
	void DisplayDebug(const UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& VerticalPosition) const;

private:
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalPosition, float& VerticalPosition);

	void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;
};

inline float UAlsCameraComponent::GetPostProcessWeight() const
{
	return PostProcessWeight;
}

inline void UAlsCameraComponent::SetPostProcessWeight(const bool NewPostProcessWeight)
{
	PostProcessWeight = UAlsMath::Clamp01(NewPostProcessWeight);
}

inline FPostProcessSettings& UAlsCameraComponent::GetPostProcessSettingsMutable()
{
	return PostProcessSettings;
}

inline bool UAlsCameraComponent::IsRightShoulder() const
{
	return bRightShoulder;
}

inline void UAlsCameraComponent::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
}
