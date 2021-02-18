#pragma once

#include "AlsGroundedEntryMode.generated.h"

UENUM(BlueprintType)
enum class EAlsGroundedEntryMode : uint8
{
	Default,
	FromRoll
};

USTRUCT(BlueprintType)
struct FAlsGroundedEntryMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsGroundedEntryMode GroundedEntryMode{EAlsGroundedEntryMode::Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bDefault{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bFromRoll{false};

public:
	FAlsGroundedEntryMode() {}

	FAlsGroundedEntryMode(const EAlsGroundedEntryMode InitialGroundedEntryMode)
	{
		*this = InitialGroundedEntryMode;
	}

	FORCEINLINE bool IsDefault() const
	{
		return bDefault;
	}

	FORCEINLINE bool IsFromRoll() const
	{
		return bFromRoll;
	}

	FORCEINLINE operator EAlsGroundedEntryMode() const
	{
		return GroundedEntryMode;
	}

	FORCEINLINE void operator=(const EAlsGroundedEntryMode NewGroundedEntryMode)
	{
		GroundedEntryMode = NewGroundedEntryMode;

		bDefault = GroundedEntryMode == EAlsGroundedEntryMode::Default;
		bFromRoll = GroundedEntryMode == EAlsGroundedEntryMode::FromRoll;
	}
};
