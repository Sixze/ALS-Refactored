#pragma once

#include "AlsCharacter.h"
#include "AlsCharacterExample.generated.h"

class UAlsCameraComponent;

UCLASS()
class ALSEXAMPLE_API AAlsCharacterExample : public AAlsCharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UAlsCameraComponent* AlsCamera;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookUpRate{45.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookRightRate{90.0f};

private:
	FTimerHandle SprintStartTimer;

public:
	AAlsCharacterExample();

	// Camera

protected:
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// Input

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputLookUpMouse(float Value);

	void InputLookRightMouse(float Value);

	void InputLookUp(float Value);

	void InputLookRight(float Value);

	void InputMoveForward(float Value);

	void InputMoveRight(float Value);

	void InputSprintPressed();

	void InputSprintReleased();

	void OnSprintStartTimerEnded();

	void InputRoll();

	void InputWalk();

	void InputCrouch();

	void InputJumpPressed();

	void InputJumpReleased();

	void InputAimPressed();

	void InputAimReleased();

	void InputRotationModePressed();

	void InputRagdollPressed();

	void InputViewModePressed();

	void InputSwitchShoulderPressed();

	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalPosition) override;
};
