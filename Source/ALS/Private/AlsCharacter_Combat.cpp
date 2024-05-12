#include "AlsCharacter.h"
#include "FSweepTraceParameters.h"
#include "Net/Core/PushModel/PushModel.h"

void AAlsCharacter::SetAttackState(const FGameplayTag& NewAttackState)
{
	SetAttackState(NewAttackState, true);
}

void AAlsCharacter::SetAttackState(const FGameplayTag& NewAttackState, const bool bSendRpc)
{
	if (AttackState == NewAttackState || GetLocalRole() < ROLE_AutonomousProxy)
	{
		return;
	}

	AttackState = NewAttackState;

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, AttackState, this)

		if (bSendRpc)
		{
			if (GetLocalRole() >= ROLE_Authority)
			{
				ClientSetAttackState(AttackState);
			}
			else
			{
				ServerSetAttackState(AttackState);
			}
		}
}

void AAlsCharacter::ClientSetAttackState_Implementation(const FGameplayTag& NewAttackState)
{
	SetAttackState(NewAttackState, false);
}

void AAlsCharacter::ServerSetAttackState_Implementation(const FGameplayTag& NewAttackState)
{
	SetAttackState(NewAttackState, false);
}

void AAlsCharacter::OnReplicated_AttackState(const FGameplayTag& PreviousAttackState)
{
	OnAttackStateChanged(PreviousAttackState);
}

void AAlsCharacter::OnAttackStateChanged_Implementation(const FGameplayTag& PreviousAttackState)
{
}

FSweepTraceParameters AAlsCharacter::CalculateSweepTraceParameters(const UMeshComponent* ComponentMesh, const FName StartSocketName, const FName EndSocketName)
{
	// 获取当前武器两端插槽的世界空间位置
	const FVector CurrentWeaponTraceStart = ComponentMesh->GetSocketLocation(StartSocketName);
	const FVector CurrentWeaponTraceEnd = ComponentMesh->GetSocketLocation(EndSocketName);

	if (WeaponTraceStart.IsNearlyZero() || WeaponTraceEnd.IsNearlyZero())
	{
		// 更新数据
		WeaponTraceStart = CurrentWeaponTraceStart;
		WeaponTraceEnd = CurrentWeaponTraceEnd;
	}
	// 获取上一帧的武器两端插槽的世界空间位置
	const FVector PreviousWeaponTraceStart = WeaponTraceStart;
	const FVector PreviousWeaponTraceEnd = WeaponTraceEnd;

	// 计算平行四边形的Trace开始点和结束点（平行四边形的近边和远边的两个中心点）
	const FVector Start = (CurrentWeaponTraceStart + PreviousWeaponTraceStart) / 2;
	const FVector End = (CurrentWeaponTraceEnd + PreviousWeaponTraceEnd) / 2;

	// 当前的WeaponTraceStart空间位置和上一帧的WeaponTraceStart空间位置是梯形的最短边，忽略这条边，用远端计算平行四边形的两条边
	const FVector Direction1 = (CurrentWeaponTraceEnd - PreviousWeaponTraceEnd).GetSafeNormal();
	const FVector Direction2 = (PreviousWeaponTraceStart - PreviousWeaponTraceEnd).GetSafeNormal();

	// 计算平行四边形的法线向量
	const FVector Normal = FVector::CrossProduct(Direction1, Direction2).GetSafeNormal();

	// 计算面对应的旋转
	const FRotator Orientation = FRotationMatrix::MakeFromZX(Normal, Direction1).Rotator();

	// 计算half size，
	FVector HalfSize;
	HalfSize.X = (CurrentWeaponTraceEnd - PreviousWeaponTraceEnd).Size() / 2; // X轴半长度
	HalfSize.Y = 0; // Y轴半长度，Y轴将增加/减少起点和终点，0则保持两端
	HalfSize.Z = 0; // Z轴半长度，由于是平面

	// 更新数据
	WeaponTraceStart = CurrentWeaponTraceStart;
	WeaponTraceEnd = CurrentWeaponTraceEnd;

	return {Start, End, HalfSize, Orientation};
}

void AAlsCharacter::ResetWeaponTraceData()
{
	WeaponTraceStart = {0.0f, 0.0f, 0.0f};
	WeaponTraceEnd = {0.0f, 0.0f, 0.0f};
}