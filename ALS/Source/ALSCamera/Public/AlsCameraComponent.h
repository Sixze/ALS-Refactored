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

	// Debug

public:
	void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& VerticalPosition) const;

private:
	static void DisplayDebugHeader(UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor,
	                               float Scale, float HorizontalPosition, float& VerticalPosition);

	void DisplayDebugCurves(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugShapes(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;

	void DisplayDebugTraces(UCanvas* Canvas, float Scale, float HorizontalPosition, float& VerticalPosition) const;
};

inline bool UAlsCameraComponent::IsRightShoulder() const
{
	return bRightShoulder;
}

inline void UAlsCameraComponent::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
}
