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
struct ALS_API FAlsMovementDirectionCache
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (AllowPrivateAccess))
	EAlsMovementDirection MovementDirection{EAlsMovementDirection::Forward};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (AllowPrivateAccess))
	uint8 bForward : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (AllowPrivateAccess))
	uint8 bBackward : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (AllowPrivateAccess))
	uint8 bLeft : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (AllowPrivateAccess))
	uint8 bRight : 1 {false};

public:
	constexpr FAlsMovementDirectionCache() = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	constexpr FAlsMovementDirectionCache(const EAlsMovementDirection NewMovementDirection)
	{
		MovementDirection = NewMovementDirection;

		bForward = MovementDirection == EAlsMovementDirection::Forward;
		bBackward = MovementDirection == EAlsMovementDirection::Backward;
		bLeft = MovementDirection == EAlsMovementDirection::Left;
		bRight = MovementDirection == EAlsMovementDirection::Right;
	}

	constexpr bool IsForward() const
	{
		return bForward;
	}

	constexpr bool IsBackward() const
	{
		return bBackward;
	}

	constexpr bool IsLeft() const
	{
		return bLeft;
	}

	constexpr bool IsRight() const
	{
		return bRight;
	}

	// ReSharper disable once CppNonExplicitConversionOperator
	constexpr operator EAlsMovementDirection() const
	{
		return MovementDirection;
	}
};
