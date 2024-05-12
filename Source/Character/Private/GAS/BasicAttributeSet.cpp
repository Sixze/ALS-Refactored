// Copyright Lubin Tu, All Rights Reserved.

#include "GAS/BasicAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

// 游戏属性集合类的预属性更改函数
void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 如果属性为生命值属性
	if (Attribute == GetHealthAttribute())
	{
		// 将新值限制在0.0和最大生命值之间
		NewValue = FMath::Clamp(NewValue, 0.0f, GetHealthMax());
	}
}

// 游戏效果执行后函数
void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 如果数据中评估的属性为伤害属性
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 获取造成的本地伤害
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		// 如果本地伤害大于0
		if (LocalDamageDone > 0.f)
		{
			// 计算新的生命值并限制在0和最大生命值之间
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetHealthMax()));
		}
	}
}

// 同步生命值属性变化
void UBasicAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Health, OldHealth);
}

// 同步最大生命值属性变化
void UBasicAttributeSet::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, HealthMax, OldHealthMax);
}

// 获取复制的寿命属性
void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 设置生命值属性的复制条件和通知类型
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health, COND_None, REPNOTIFY_Always);
}