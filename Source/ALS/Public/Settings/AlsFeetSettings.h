#pragma once

#include "Engine/EngineTypes.h"
#include "AlsFeetSettings.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsFeetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	bool bDisableFootLock{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float FootHeight{13.5f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS")
	TEnumAsByte<ETraceTypeQuery> IkTraceChannel{TraceTypeQuery1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float IkTraceDistanceUpward{50.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float IkTraceDistanceDownward{45.0f};
};
