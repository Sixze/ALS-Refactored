#pragma once

#include "AlsRotationMode.generated.h"

UENUM(BlueprintType)
enum class EAlsRotationMode : uint8
{
	LookingDirection,
	VelocityDirection,
	Aiming
};

USTRUCT(BlueprintType)
struct FAlsRotationMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsRotationMode RotationMode{EAlsRotationMode::LookingDirection};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bLookingDirection{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bVelocityDirection{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bAiming{false};

public:
	FAlsRotationMode() {}

	FAlsRotationMode(const EAlsRotationMode InitialRotationMode)
	{
		*this = InitialRotationMode;
	}

	bool IsLookingDirection() const
	{
		return bLookingDirection;
	}

	bool IsVelocityDirection() const
	{
		return bVelocityDirection;
	}

	bool IsAiming() const
	{
		return bAiming;
	}

	operator EAlsRotationMode() const
	{
		return RotationMode;
	}

	void operator=(const EAlsRotationMode NewRotationMode)
	{
		RotationMode = NewRotationMode;

		bLookingDirection = RotationMode == EAlsRotationMode::LookingDirection;
		bVelocityDirection = RotationMode == EAlsRotationMode::VelocityDirection;
		bAiming = RotationMode == EAlsRotationMode::Aiming;
	}
};
