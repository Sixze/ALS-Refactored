#pragma once

#include "AlsLocomotionMode.generated.h"

UENUM(BlueprintType)
enum class EAlsLocomotionMode : uint8
{
	None,
	Grounded,
	InAir,
};

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsLocomotionMode LocomotionMode{EAlsLocomotionMode::Grounded};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bNone{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bGrounded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bInAir{false};

public:
	FAlsLocomotionMode() = default;

	FAlsLocomotionMode(const EAlsLocomotionMode InitialLocomotionMode)
	{
		*this = InitialLocomotionMode;
	}

	bool IsNone() const
	{
		return bNone;
	}

	bool IsGrounded() const
	{
		return bGrounded;
	}

	bool IsInAir() const
	{
		return bInAir;
	}

	operator EAlsLocomotionMode() const
	{
		return LocomotionMode;
	}

	void operator=(const EAlsLocomotionMode NewLocomotionMode)
	{
		LocomotionMode = NewLocomotionMode;

		bNone = LocomotionMode == EAlsLocomotionMode::None;
		bGrounded = LocomotionMode == EAlsLocomotionMode::Grounded;
		bInAir = LocomotionMode == EAlsLocomotionMode::InAir;
	}
};
