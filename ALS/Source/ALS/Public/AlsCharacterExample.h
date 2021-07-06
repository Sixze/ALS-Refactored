#pragma once

#include "AlsCharacter.h"

#include "AlsCharacterExample.generated.h"

UCLASS()
class ALS_API AAlsCharacterExample : public AAlsCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookUpRate{45.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Example", Meta = (AllowPrivateAccess, ClampMin = 0))
	float LookRightRate{90.0f};

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

	void InputRoll();

	void InputWalk();

	void InputCrouch();

	void InputJumpPressed();

	void InputJumpReleased();

	void InputAimPressed();

	void InputAimReleased();

	void VelocityDirectionPressed();

	void LookingDirectionPressed();

	void RagdollPressed();

	void ViewModePressed();
};
