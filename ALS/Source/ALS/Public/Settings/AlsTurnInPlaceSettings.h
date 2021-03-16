#pragma once

#include "AlsTurnInPlaceSettings.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct ALS_API FAlsTurnInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float PlayRate{1.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bScalePlayRateByAnimatedTurnAngle{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180, ClampMax = 180))
	float AnimatedTurnAngle;
};

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralTurnInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 180))
	float AimingYawAngleThreshold{45.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AimingYawSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	FVector2D AimingYawAngleToActivationDelay{0, 0.75f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 180, DisplayName="Turn 180 Angle Threshold"))
	float Turn180AngleThreshold{130.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Standing Turn 90 Left"))
	FAlsTurnInPlaceSettings StandingTurn90Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Standing Turn 90 Right"))
	FAlsTurnInPlaceSettings StandingTurn90Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Standing Turn 180 Left"))
	FAlsTurnInPlaceSettings StandingTurn180Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Standing Turn 180 Right"))
	FAlsTurnInPlaceSettings StandingTurn180Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Crouching Turn 90 Left"))
	FAlsTurnInPlaceSettings CrouchingTurn90Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Crouching Turn 90 Right"))
	FAlsTurnInPlaceSettings CrouchingTurn90Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Crouching Turn 180 Left"))
	FAlsTurnInPlaceSettings CrouchingTurn180Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName="Crouching Turn 180 Right"))
	FAlsTurnInPlaceSettings CrouchingTurn180Right;
};
