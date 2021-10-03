#pragma once

#include "AlsLocomotionMode.generated.h"

UENUM(BlueprintType)
enum class EAlsLocomotionMode : uint8
{
	Grounded,
	InAir,
	Mantling,
	Ragdolling
};

USTRUCT(BlueprintType)
struct FAlsLocomotionMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsLocomotionMode LocomotionMode{EAlsLocomotionMode::Grounded};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bGrounded{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bInAir{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bMantling{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bRagdolling{false};

public:
	FAlsLocomotionMode() = default;

	FAlsLocomotionMode(const EAlsLocomotionMode InitialLocomotionMode)
	{
		*this = InitialLocomotionMode;
	}

	bool IsGrounded() const
	{
		return bGrounded;
	}

	bool IsInAir() const
	{
		return bInAir;
	}

	bool IsMantling() const
	{
		return bMantling;
	}

	bool IsRagdolling() const
	{
		return bRagdolling;
	}

	operator EAlsLocomotionMode() const
	{
		return LocomotionMode;
	}

	void operator=(const EAlsLocomotionMode NewLocomotionMode)
	{
		LocomotionMode = NewLocomotionMode;

		bGrounded = LocomotionMode == EAlsLocomotionMode::Grounded;
		bInAir = LocomotionMode == EAlsLocomotionMode::InAir;
		bMantling = LocomotionMode == EAlsLocomotionMode::Mantling;
		bRagdolling = LocomotionMode == EAlsLocomotionMode::Ragdolling;
	}
};
