#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsMath.h"
#include "AlsCameraComponent.generated.h"

class UAlsCameraSettings;
class ACharacter;

UCLASS(HideCategories = ("ComponentTick", "Clothing", "Physics", "MasterPoseComponent", "Collision", "AnimationRig",
	"Lighting", "Deformer", "Rendering", "PathTracing", "HLOD", "Navigation", "VirtualTexture", "SkeletalMesh",
	"LeaderPoseComponent", "Optimization", "LOD", "MaterialParameters", "TextureStreaming", "Mobile", "RayTracing"))
class ALSCAMERA_API UAlsCameraComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UAlsCameraSettings> Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (ClampMin = 0, ClampMax = 1))
	float PostProcessWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<ACharacter> Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ShowInnerProperties))
	TWeakObjectPtr<UAnimInstance> AnimationInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ForceUnits = "x"))
	float PreviousGlobalTimeDilation{1.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PivotTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PivotLagLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PivotLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector CameraLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FRotator CameraRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<UPrimitiveComponent> MovementBasePrimitive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FName MovementBaseBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PivotMovementBaseRelativeLagLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FQuat CameraMovementBaseRelativeRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 0, ClampMax = 1, ForceUnits = "%"))
	float TraceDistanceRatio{1.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (ClampMin = 5, ClampMax = 360, ForceUnits = "deg"))
	float CameraFov{90.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bRightShoulder{true};

public:
	UAlsCameraComponent();

	virtual void OnRegister() override;

	virtual void RegisterComponentTickFunctions(bool bRegister) override;

	virtual void Activate(bool bReset) override;

	virtual void InitAnim(bool bForceReinitialize) override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void CompleteParallelAnimationEvaluation(bool bDoPostAnimationEvaluation) override;

public:
	float GetPostProcessWeight() const;

	void SetPostProcessWeight(float NewPostProcessWeight);

	bool IsRightShoulder() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Camera")
	void SetRightShoulder(bool bNewRightShoulder);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera", Meta = (ReturnDisplayName = "Camera Location"))
	FVector GetFirstPersonCameraLocation() const;

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera", Meta = (ReturnDisplayName = "Pivot Location"))
	FVector GetThirdPersonPivotLocation() const;

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera", Meta = (ReturnDisplayName = "Trace Start"))
	FVector GetThirdPersonTraceStartLocation() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Camera")
	void GetViewInfo(FMinimalViewInfo& ViewInfo) const;

private:
	void TickCamera(float DeltaTime, bool bAllowLag = true);

	FRotator CalculateCameraRotation(const FRotator& CameraTargetRotation, float DeltaTime, bool bAllowLag) const;

	FVector CalculatePivotLagLocation(const FQuat& CameraYawRotation, float DeltaTime, bool bAllowLag) const;

	FVector CalculatePivotOffset() const;

	FVector CalculateCameraOffset() const;

	FVector CalculateCameraTrace(const FVector& CameraTargetLocation, const FVector& PivotOffset,
	                             float DeltaTime, bool bAllowLag, float& NewTraceDistanceRatio) const;

	bool TryAdjustLocationBlockedByGeometry(FVector& Location, bool bDisplayDebugCameraTraces) const;

	// Debug

public:
	void DisplayDebug(const UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& VerticalLocation) const;

private:
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalLocation, float& VerticalLocation);

	void DisplayDebugCurves(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugShapes(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

	void DisplayDebugTraces(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;
};

inline float UAlsCameraComponent::GetPostProcessWeight() const
{
	return PostProcessWeight;
}

inline void UAlsCameraComponent::SetPostProcessWeight(const float NewPostProcessWeight)
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
