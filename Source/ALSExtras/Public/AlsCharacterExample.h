#pragma once

#include "AlsCharacter.h"
#include "AlsCharacterExample.generated.h"

struct FInputActionValue;
class UAlsCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS(AutoExpandCategories = ("Settings|Als Character Example", "State|Als Character Example"))
class ALSEXTRAS_API AAlsCharacterExample : public AAlsCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Als Character Example")
	TObjectPtr<UAlsCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RagdollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RotationModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ViewModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character Example", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SwitchShoulderAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Als Character Example",
		Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookUpRate{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Als Character Example",
		Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookRightRate{240.0f};

public:
	AAlsCharacterExample();

	virtual void NotifyControllerChanged() override;

	// Camera

protected:
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// Input

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

private:
	void Input_OnLookMouse(const FInputActionValue& ActionValue);

	void Input_OnLook(const FInputActionValue& ActionValue);

	void Input_OnMove(const FInputActionValue& ActionValue);

	void Input_OnSprint(const FInputActionValue& ActionValue);

	void Input_OnWalk();

	void Input_OnCrouch();

	void Input_OnJump(const FInputActionValue& ActionValue);

	void Input_OnAim(const FInputActionValue& ActionValue);

	void Input_OnRagdoll();

	void Input_OnRoll();

	void Input_OnRotationMode();

	void Input_OnViewMode();

	void Input_OnSwitchShoulder();

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;
};
