#pragma once

#include "AlsMovementDirection.generated.h"

UENUM(BlueprintType)
enum class EAlsMovementDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

USTRUCT(BlueprintType)
struct FAlsMovementDirection
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsMovementDirection MovementDirection{EAlsMovementDirection::Forward};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bForward{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bBackward{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bLeft{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bRight{false};

public:
	FAlsMovementDirection() {}

	FAlsMovementDirection(const EAlsMovementDirection InitialMovementDirection)
	{
		*this = InitialMovementDirection;
	}

	FORCEINLINE bool IsForward() const
	{
		return bForward;
	}

	FORCEINLINE bool IsBackward() const
	{
		return bBackward;
	}

	FORCEINLINE bool IsLeft() const
	{
		return bLeft;
	}

	FORCEINLINE bool IsRight() const
	{
		return bRight;
	}

	FORCEINLINE operator EAlsMovementDirection() const
	{
		return MovementDirection;
	}

	FORCEINLINE void operator=(const EAlsMovementDirection NewMovementDirection)
	{
		MovementDirection = NewMovementDirection;

		bForward = MovementDirection == EAlsMovementDirection::Forward;
		bBackward = MovementDirection == EAlsMovementDirection::Backward;
		bLeft = MovementDirection == EAlsMovementDirection::Left;
		bRight = MovementDirection == EAlsMovementDirection::Right;
	}
};
