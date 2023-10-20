#pragma once

#include "Engine/EngineTypes.h"
#include "AlsFeetSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsFootLimitsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FVector3f TwistAxis{1.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float TwistLimitAngle{180.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FRotator3f SwingLimitOffset{ForceInit};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		DisplayName = "Swing 1 Limit Angle", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float Swing1LimitAngle{180.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS",
		DisplayName = "Swing 2 Limit Angle", Meta = (ClampMin = 0, ClampMax = 180, ForceUnits = "deg"))
	float Swing2LimitAngle{180.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay, Meta = (ClampMin = 0, ClampMax = 1))
	float TwistLimit{1.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS", AdvancedDisplay)
	FQuat4f SwingLimitOffsetQuaternion{ForceInit};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS",
		DisplayName = "Swing 1 Limit", AdvancedDisplay, Meta = (ClampMin = 0, ClampMax = 1))
	float Swing1Limit{1.0f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ALS",
		DisplayName = "Swing 2 Limit", AdvancedDisplay, Meta = (ClampMin = 0, ClampMax = 1))
	float Swing2Limit{1.0f};

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
#endif
};

USTRUCT(BlueprintType)
struct ALS_API FAlsFeetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bDisableFootLock{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float FootHeight{13.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TEnumAsByte<ECollisionChannel> IkTraceChannel{ECC_Visibility};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float IkTraceDistanceUpward{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float IkTraceDistanceDownward{45.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FAlsFootLimitsSettings LeftFootLimits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	FAlsFootLimitsSettings RightFootLimits;

public:
#if WITH_EDITOR
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
