#pragma once

#include "AlsLocomotionAction.generated.h"

UENUM(BlueprintType)
enum class EAlsLocomotionAction : uint8
{
	None,
	Mantling,
	Ragdolling,
	GettingUp,
	Rolling
};

USTRUCT(BlueprintType)
struct ALS_API FAlsLocomotionAction
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsLocomotionAction LocomotionAction{EAlsLocomotionAction::None};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bNone{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bMantling{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bRagdolling{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bGettingUp{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bRolling{false};

public:
	FAlsLocomotionAction() = default;

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

	bool IsRagdolling() const
	{
		return bRagdolling;
	}

	bool IsGettingUp() const
	{
		return bGettingUp;
	}

	bool IsRolling() const
	{
		return bRolling;
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
		bRagdolling = LocomotionAction == EAlsLocomotionAction::Ragdolling;
		bGettingUp = LocomotionAction == EAlsLocomotionAction::GettingUp;
		bRolling = LocomotionAction == EAlsLocomotionAction::Rolling;
	}
};
