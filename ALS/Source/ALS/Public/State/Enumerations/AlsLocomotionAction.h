#pragma once

#include "AlsLocomotionAction.generated.h"

UENUM(BlueprintType)
enum class EAlsLocomotionAction : uint8
{
	None,
	Mantling,
	Rolling,
	GettingUp
};

USTRUCT(BlueprintType)
struct FAlsLocomotionAction
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsLocomotionAction LocomotionAction{EAlsLocomotionAction::None};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bNone{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bMantling{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bRolling{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bGettingUp{false};

public:
	FAlsLocomotionAction() {}

	FAlsLocomotionAction(const EAlsLocomotionAction InitialLocomotionAction)
	{
		*this = InitialLocomotionAction;
	}

	bool IsNone() const
	{
		return bNone;
	}

	bool IsMantling() const
	{
		return bMantling;
	}

	bool IsRolling() const
	{
		return bRolling;
	}

	bool IsGettingUp() const
	{
		return bGettingUp;
	}

	operator EAlsLocomotionAction() const
	{
		return LocomotionAction;
	}

	void operator=(const EAlsLocomotionAction NewLocomotionAction)
	{
		LocomotionAction = NewLocomotionAction;

		bNone = LocomotionAction == EAlsLocomotionAction::None;
		bMantling = LocomotionAction == EAlsLocomotionAction::Mantling;
		bRolling = LocomotionAction == EAlsLocomotionAction::Rolling;
		bGettingUp = LocomotionAction == EAlsLocomotionAction::GettingUp;
	}
};
