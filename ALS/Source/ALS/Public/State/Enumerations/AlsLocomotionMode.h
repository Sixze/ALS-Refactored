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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsLocomotionMode LocomotionMode{EAlsLocomotionMode::Grounded};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bGrounded{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bInAir{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bMantling{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bRagdolling{false};

public:
	FAlsLocomotionMode() {}

	FAlsLocomotionMode(const EAlsLocomotionMode InitialLocomotionMode)
	{
		*this = InitialLocomotionMode;
	}

	FORCEINLINE bool IsGrounded() const
	{
		return bGrounded;
	}

	FORCEINLINE bool IsInAir() const
	{
		return bInAir;
	}

	FORCEINLINE bool IsMantling() const
	{
		return bMantling;
	}

	FORCEINLINE bool IsRagdolling() const
	{
		return bRagdolling;
	}

	FORCEINLINE operator EAlsLocomotionMode() const
	{
		return LocomotionMode;
	}

	FORCEINLINE void operator=(const EAlsLocomotionMode NewLocomotionMode)
	{
		LocomotionMode = NewLocomotionMode;

		bGrounded = LocomotionMode == EAlsLocomotionMode::Grounded;
		bInAir = LocomotionMode == EAlsLocomotionMode::InAir;
		bMantling = LocomotionMode == EAlsLocomotionMode::Mantling;
		bRagdolling = LocomotionMode == EAlsLocomotionMode::Ragdolling;
	}
};
