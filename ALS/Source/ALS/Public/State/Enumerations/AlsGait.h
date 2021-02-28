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
struct FAlsGait
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsGait Gait{EAlsGait::Walking};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bWalking{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bRunning{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bSprinting{false};

public:
	FAlsGait() {}

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
