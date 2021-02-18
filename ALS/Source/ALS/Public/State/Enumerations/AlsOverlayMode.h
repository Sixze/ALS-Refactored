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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EAlsOverlayMode OverlayMode{EAlsOverlayMode::Default};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bDefault{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bMasculine{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bFeminine{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bInjured{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bHandsTied{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bM4{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bPistolOneHanded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bPistolTwoHanded{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bBow{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bTorch{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bBinoculars{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bBox{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bBarrel{false};

public:
	FAlsOverlayMode() {}

	FAlsOverlayMode(const EAlsOverlayMode InitialOverlayMode)
	{
		*this = InitialOverlayMode;
	}

	FORCEINLINE bool IsDefault() const
	{
		return bDefault;
	}

	FORCEINLINE bool IsMasculine() const
	{
		return bMasculine;
	}

	FORCEINLINE bool IsFeminine() const
	{
		return bFeminine;
	}

	FORCEINLINE bool IsInjured() const
	{
		return bInjured;
	}

	FORCEINLINE bool IsHandsTied() const
	{
		return bHandsTied;
	}

	FORCEINLINE bool IsM4() const
	{
		return bM4;
	}

	FORCEINLINE bool IsPistolOneHanded() const
	{
		return bPistolOneHanded;
	}

	FORCEINLINE bool IsPistolTwoHanded() const
	{
		return bPistolTwoHanded;
	}

	FORCEINLINE bool IsBow() const
	{
		return bBow;
	}

	FORCEINLINE bool IsTorch() const
	{
		return bTorch;
	}

	FORCEINLINE bool IsBinoculars() const
	{
		return bBinoculars;
	}

	FORCEINLINE bool IsBox() const
	{
		return bBox;
	}

	FORCEINLINE bool IsBarrel() const
	{
		return bBarrel;
	}

	FORCEINLINE operator EAlsOverlayMode() const
	{
		return OverlayMode;
	}

	FORCEINLINE void operator=(const EAlsOverlayMode NewOverlayMode)
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
