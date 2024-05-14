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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bForward : 1 {true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bBackward : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bLeft : 1 {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	uint8 bRight : 1 {false};

public:
	constexpr FAlsMovementDirectionCache() = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	constexpr FAlsMovementDirectionCache(const EAlsMovementDirection MovementDirection)
	{
		bForward = MovementDirection == EAlsMovementDirection::Forward;
		bBackward = MovementDirection == EAlsMovementDirection::Backward;
		bLeft = MovementDirection == EAlsMovementDirection::Left;
		bRight = MovementDirection == EAlsMovementDirection::Right;
	}
};
