#pragma once

#include "AlsTurnInPlaceSettings.generated.h"

class UAnimSequenceBase;

UCLASS(BlueprintType, EditInlineNew)
class ALS_API UAlsTurnInPlaceSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "x"))
	float PlayRate{1.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bScalePlayRateByAnimatedTurnAngle{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float AnimatedTurnAngle;
};

USTRUCT(BlueprintType)
struct ALS_API FAlsGeneralTurnInPlaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float ViewYawAngleThreshold{45.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float ViewYawSpeedThreshold{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	FVector2D ViewYawAngleToActivationDelay{0.0f, 0.75f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Turn 180 Angle Threshold",
		Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float Turn180AngleThreshold{130.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ForceUnits = "s"))
	float BlendTime{0.2f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisableFootLock{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Standing Turn 90 Left")
	TObjectPtr<UAlsTurnInPlaceSettings> StandingTurn90Left{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Standing Turn 90 Right")
	TObjectPtr<UAlsTurnInPlaceSettings> StandingTurn90Right{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Standing Turn 180 Left")
	TObjectPtr<UAlsTurnInPlaceSettings> StandingTurn180Left{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Standing Turn 180 Right")
	TObjectPtr<UAlsTurnInPlaceSettings> StandingTurn180Right{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Crouching Turn 90 Left")
	TObjectPtr<UAlsTurnInPlaceSettings> CrouchingTurn90Left{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Crouching Turn 90 Right")
	TObjectPtr<UAlsTurnInPlaceSettings> CrouchingTurn90Right{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Crouching Turn 180 Left")
	TObjectPtr<UAlsTurnInPlaceSettings> CrouchingTurn180Left{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, DisplayName = "Crouching Turn 180 Right")
	TObjectPtr<UAlsTurnInPlaceSettings> CrouchingTurn180Right{nullptr};
};
