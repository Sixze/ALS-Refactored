#pragma once

#include "AlsOverlayMode.generated.h"

UENUM(BlueprintType)
enum class EAlsOverlayMode : uint8
{
	Default,
	Masculine,
	Feminine,
	Injured,
	HandsTied,
	M4,
	PistolOneHanded,
	PistolTwoHanded,
	Bow,
	Torch,
	Binoculars,
	Box,
	Barrel
};

USTRUCT(BlueprintType)
struct FAlsOverlayMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	EAlsOverlayMode OverlayMode{EAlsOverlayMode::Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bDefault{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bMasculine{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bFeminine{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bInjured{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bHandsTied{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bM4{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bPistolOneHanded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bPistolTwoHanded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bBow{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bTorch{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bBinoculars{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bBox{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess))
	bool bBarrel{false};

public:
	FAlsOverlayMode() {}

	FAlsOverlayMode(const EAlsOverlayMode InitialOverlayMode)
	{
		*this = InitialOverlayMode;
	}

	bool IsDefault() const
	{
		return bDefault;
	}

	bool IsMasculine() const
	{
		return bMasculine;
	}

	bool IsFeminine() const
	{
		return bFeminine;
	}

	bool IsInjured() const
	{
		return bInjured;
	}

	bool IsHandsTied() const
	{
		return bHandsTied;
	}

	bool IsM4() const
	{
		return bM4;
	}

	bool IsPistolOneHanded() const
	{
		return bPistolOneHanded;
	}

	bool IsPistolTwoHanded() const
	{
		return bPistolTwoHanded;
	}

	bool IsBow() const
	{
		return bBow;
	}

	bool IsTorch() const
	{
		return bTorch;
	}

	bool IsBinoculars() const
	{
		return bBinoculars;
	}

	bool IsBox() const
	{
		return bBox;
	}

	bool IsBarrel() const
	{
		return bBarrel;
	}

	operator EAlsOverlayMode() const
	{
		return OverlayMode;
	}

	void operator=(const EAlsOverlayMode NewOverlayMode)
	{
		OverlayMode = NewOverlayMode;

		bDefault = OverlayMode == EAlsOverlayMode::Default;
		bMasculine = OverlayMode == EAlsOverlayMode::Masculine;
		bFeminine = OverlayMode == EAlsOverlayMode::Feminine;
		bInjured = OverlayMode == EAlsOverlayMode::Injured;
		bHandsTied = OverlayMode == EAlsOverlayMode::HandsTied;
		bM4 = OverlayMode == EAlsOverlayMode::M4;
		bPistolOneHanded = OverlayMode == EAlsOverlayMode::PistolOneHanded;
		bPistolTwoHanded = OverlayMode == EAlsOverlayMode::PistolTwoHanded;
		bBow = OverlayMode == EAlsOverlayMode::Bow;
		bTorch = OverlayMode == EAlsOverlayMode::Torch;
		bBinoculars = OverlayMode == EAlsOverlayMode::Binoculars;
		bBox = OverlayMode == EAlsOverlayMode::Box;
		bBarrel = OverlayMode == EAlsOverlayMode::Barrel;
	}
};
