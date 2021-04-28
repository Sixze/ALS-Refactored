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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsGroundedEntryMode GroundedEntryMode{EAlsGroundedEntryMode::Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bDefault{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bFromRoll{false};

public:
	FAlsGroundedEntryMode() {}

	FAlsGroundedEntryMode(const EAlsGroundedEntryMode InitialGroundedEntryMode)
	{
		*this = InitialGroundedEntryMode;
	}

	bool IsDefault() const
	{
		return bDefault;
	}

	bool IsFromRoll() const
	{
		return bFromRoll;
	}

	operator EAlsGroundedEntryMode() const
	{
		return GroundedEntryMode;
	}

	void operator=(const EAlsGroundedEntryMode NewGroundedEntryMode)
	{
		GroundedEntryMode = NewGroundedEntryMode;

		bDefault = GroundedEntryMode == EAlsGroundedEntryMode::Default;
		bFromRoll = GroundedEntryMode == EAlsGroundedEntryMode::FromRoll;
	}
};
