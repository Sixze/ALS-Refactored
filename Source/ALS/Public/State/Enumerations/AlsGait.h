#pragma once

#include "AlsGait.generated.h"

UENUM(BlueprintType)
enum class EAlsGait : uint8
{
	Walking,
	Running,
	Sprinting
};

USTRUCT(BlueprintType)
struct ALS_API FAlsGait
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsGait Gait{EAlsGait::Walking};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bWalking{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bRunning{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bSprinting{false};

public:
	FAlsGait() = default;

	FAlsGait(const EAlsGait InitialGait)
	{
		*this = InitialGait;
	}

	bool IsWalking() const
	{
		return bWalking;
	}

	bool IsRunning() const
	{
		return bRunning;
	}

	bool IsSprinting() const
	{
		return bSprinting;
	}

	operator EAlsGait() const
	{
		return Gait;
	}

	void operator=(const EAlsGait NewGait)
	{
		Gait = NewGait;

		bWalking = Gait == EAlsGait::Walking;
		bRunning = Gait == EAlsGait::Running;
		bSprinting = Gait == EAlsGait::Sprinting;
	}
};
