#pragma once

#include "FSweepTraceParameters.generated.h"

// 武器的盒型线条扫描所需要的数据，用于检测命中
USTRUCT(BlueprintType)
struct FSweepTraceParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Start{0.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector End{0.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HalfSize{0.0f, 0.0f, 0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotator{0.0f, 0.0f, 0.0f};
};
