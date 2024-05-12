#include "AlsAnimationInstance.h"
#include "AlsCharacter.h"

void UAlsAnimationInstance::RefreshCombatStats()
{
	// ToDo:部分数值目前写死，后面动画完善再改

	// 无需判断击中直接返回
	if (!Character->IsAbleToHit())
	{
		// 还原默认值
		CombatStats.AttackSpeed = 1.0f;
		CombatStats.AttackSpeedOnHit = 1.0f;
		CombatStats.AttackSpeedAfterHit = 1.0f;
		return;
	}
	
	// 正在击中物体状态，并且没有减少过速度
	if (Character->IsHitting())
	{
		// 被反弹
		if (Character->IsHitReflected())
		{
			CombatStats.AttackSpeedOnHit = -0.5f;
			CombatStats.AttackSpeedAfterHit = -0.1f;
		}
		// 无反弹
		else
		{
			CombatStats.AttackSpeedOnHit = 0.05f;
			CombatStats.AttackSpeedAfterHit = 0.8f;
		}
		// 应用击中物体时的攻击速度
		CombatStats.AttackSpeed = CombatStats.AttackSpeedOnHit;
	}
	// 不在击中状态但是正在攻击
	else if (Character->IsAbleToHit())
	{
		// 应用击中物体后的攻击速度(如果没击中过则保持默认)
		CombatStats.AttackSpeed = CombatStats.AttackSpeedAfterHit;
	}
}