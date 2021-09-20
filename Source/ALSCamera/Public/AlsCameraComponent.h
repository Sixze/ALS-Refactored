#pragma once

#include "Camera/CameraTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utility/AlsConstants.h"

#include "AlsCameraComponent.generated.h"

class AAlsCharacter;

UCLASS()
class ALSCAMERA_API UAlsCameraComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", AdvancedDisplay, Meta = (AllowPrivateAccess))
	bool bUseLagSubstepping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", AdvancedDisplay,
		Meta = (AllowPrivateAccess, EditCondition = "bUseCameraLagSubstepping", ClampMin = 0.005, ClampMax = 0.5))
	float MaxLagSubstepDeltaTime{1.0f / 60.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|First Person",
		Meta = (AllowPrivateAccess, ClampMin = 5, ClampMax = 360))
	float FirstPersonFov{90.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|First Person", Meta = (AllowPrivateAccess))
	FName FirstPersonSocket{TEXT("FirstPersonCamera")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person",
		Meta = (AllowPrivateAccess, ClampMin = 5, ClampMax = 360))
	float ThirdPersonFov{90.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess))
	FName ThirdPersonFirstSocket{UAlsConstants::RootBone()};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess))
	FName ThirdPersonSecondSocket{UAlsConstants::HeadBone()};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess, ClampMin = 0))
	float ThirdPersonTraceRadius{15.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess))
	TEnumAsByte<ETraceTypeQuery> ThirdPersonTraceChannel{UEngineTypes::ConvertToTraceType(ECC_Camera)};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess))
	FName ThirdPersonTraceLeftSocket{TEXT("ThirdPersonCameraTraceLeft")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess))
	FName ThirdPersonTraceRightSocket{TEXT("ThirdPersonCameraTraceRight")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Third Person", Meta = (AllowPrivateAccess))
	FVector ThirdPersonTraceOverrideOffset{0.0f, 0.0f, 40.0f};

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

	bool IsRightShoulder() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Camera")
	void SetRightShoulder(bool bNewRightShoulder);

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera")
	FVector GetFirstPersonPivotLocation() const;

	UFUNCTION(BlueprintPure, Category = "ALS|Als Camera")
	FTransform GetThirdPersonPivotTransform() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Als Camera")
	void GetViewInfo(FMinimalViewInfo& ViewInfo) const;

private:
	void TickCamera(float DeltaTime, bool bAllowLag = true);

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

inline bool UAlsCameraComponent::IsRightShoulder() const
{
	return bRightShoulder;
}

inline void UAlsCameraComponent::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
}
