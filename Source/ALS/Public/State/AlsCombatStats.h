#pragma once

#include "AlsCombatStats.generated.h"

USTRUCT(BlueprintType)
struct ALS_API FAlsCombatStats
{
	GENERATED_BODY()

	// 攻击动画速度，默认为1.0f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float AttackSpeed{1.0f};

	// 击中时的攻击动画速度，负值为反弹武器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = -1, ClampMax = 1))
	float AttackSpeedOnHit{1.0f};

	// 击中后的后续攻击动画速度，默认为1.0f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS", Meta = (ClampMin = 0, ClampMax = 1))
	float AttackSpeedAfterHit{1.0f};
};