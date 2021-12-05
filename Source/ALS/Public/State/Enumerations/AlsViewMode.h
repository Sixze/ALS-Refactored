#pragma once

#include "AlsViewMode.generated.h"

UENUM(BlueprintType)
enum class EAlsViewMode : uint8
{
	FirstPerson,
	ThirdPerson
};

USTRUCT(BlueprintType)
struct ALS_API FAlsViewMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsViewMode ViewMode{EAlsViewMode::FirstPerson};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bFirstPerson{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bThirdPerson{false};

public:
	FAlsViewMode() = default;

	FAlsViewMode(const EAlsViewMode InitialGroundedEntryMode)
	{
		*this = InitialGroundedEntryMode;
	}

	bool IsFirstPerson() const
	{
		return bFirstPerson;
	}

	bool IsThirdPerson() const
	{
		return bThirdPerson;
	}

	operator EAlsViewMode() const
	{
		return ViewMode;
	}

	void operator=(const EAlsViewMode NewGroundedEntryMode)
	{
		ViewMode = NewGroundedEntryMode;

		bFirstPerson = ViewMode == EAlsViewMode::FirstPerson;
		bThirdPerson = ViewMode == EAlsViewMode::ThirdPerson;
	}
};
