#pragma once

#include "AlsStance.generated.h"

UENUM(BlueprintType)
enum class EAlsStance : uint8
{
	Standing,
	Crouching
};

USTRUCT(BlueprintType)
struct ALS_API FAlsStance
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsStance Stance{EAlsStance::Standing};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bStanding{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bCrouching{false};

public:
	FAlsStance() = default;

	FAlsStance(const EAlsStance InitialStance)
	{
		*this = InitialStance;
	}

	bool IsStanding() const
	{
		return bStanding;
	}

	bool IsCrouching() const
	{
		return bCrouching;
	}

	operator EAlsStance() const
	{
		return Stance;
	}

	void operator=(const EAlsStance NewStance)
	{
		Stance = NewStance;

		bStanding = Stance == EAlsStance::Standing;
		bCrouching = Stance == EAlsStance::Crouching;
	}
};
