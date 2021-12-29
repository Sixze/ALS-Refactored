#pragma once

#include "Camera/CameraTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsMath.h"

#include "AlsCameraComponent.generated.h"

class UAlsCameraSettings;
class AAlsCharacter;

UCLASS()
class ALSCAMERA_API UAlsCameraComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	UAlsCameraSettings* Settings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess, ClampMin = 0, ClampMax = 1))
	float PostProcessWeight;

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

	virtual void OnRegister() override;

	virtual void Activate(bool bReset) override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetPostProcessWeight() const;

	void SetPostProcessWeight(bool NewPostProcessWeight);

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

inline bool UAlsCameraComponent::IsRightShoulder() const
{
	return bRightShoulder;
}

inline void UAlsCameraComponent::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
}
