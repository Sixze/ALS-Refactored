#pragma once

#include "AlsCharacter.h"

#include "AlsCharacterExample.generated.h"

UCLASS()
class ALS_API AAlsCharacterExample : public AAlsCharacter
{
	GENERATED_BODY()

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void InputMoveForward(float Value);

	void InputMoveRight(float Value);

	void InputLookUp(float Value);

	void InputSprintPressed();

	void InputSprintReleased();

	void InputRoll();

	void InputWalk();

	void InputCrouch();

	void InputJumpPressed();

	void InputJumpReleased();

	void InputAimPressed();

	void InputAimReleased();

	void VelocityDirectionPressedAction();

	void LookingDirectionPressedAction();

	void RagdollPressedAction();
};
