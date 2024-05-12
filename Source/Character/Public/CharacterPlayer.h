// Copyright Lubin Tu, All Rights Reserved.
#pragma once


#include "AbilitySystemInterface.h"
#include "AlsCharacter.h"
#include "GAS/BasicAttributeSet.h"
#include "CharacterPlayer.generated.h"

struct FInputActionValue;
class UAlsCameraComponent;
class UInputMappingContext;
class UInputAction;

/**
 * 包含了GAS系统的基础玩家角色类，继承自AlsCharacter
 */

UCLASS(AutoExpandCategories = ("Settings|Character", "State|Character"), Blueprintable, Meta = (ToolTip = "包含了GAS系统的基础玩家角色类，继承自AlsCharacter"))
class CHARACTER_API ACharacterPlayer : public AAlsCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterPlayer();

	virtual void NotifyControllerChanged() override;
	virtual void BeginPlay() override;

	// 角色的血量值
	float GetHealth() const;
	// 初始化属性
	void InitializeAttributes();
	// 当血量属性发生改变时调用
	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	// 死亡
	void OnDeath();

	// 覆写IAbilitySystemInterface中的函数，用来获取角色的能力系统组件
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	// Debug
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAlsCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RagdollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RotationModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ViewModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SwitchShoulderAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Character", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Character", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Character",
		Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookUpRate{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Character",
		Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookRightRate{240.0f};

	// 计算摄像头位置和朝向
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// 设置玩家输入组件
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

	// GAS相关
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|GAS", Meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|GAS", Meta = (AllowPrivateAccess = "true"))
	const UBasicAttributeSet* BasicAttributes{nullptr};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|GAS")
	TSubclassOf<UGameplayEffect> InitialGameplayEffect{nullptr};

	UPROPERTY()
	FTimerHandle OnDeathTimerHandle;

private:
	void Input_OnLookMouse(const FInputActionValue& ActionValue);

	void Input_OnLook(const FInputActionValue& ActionValue);

	void Input_OnMove(const FInputActionValue& ActionValue);

	void Input_OnSprint(const FInputActionValue& ActionValue);

	void Input_OnWalk();

	void Input_OnCrouch();

	void Input_OnJump(const FInputActionValue& ActionValue);

	void Input_OnAim(const FInputActionValue& ActionValue);

	void Input_OnRagdoll();

	void Input_OnRoll();

	void Input_OnRotationMode();

	void Input_OnViewMode();

	void Input_OnSwitchShoulder();

	void Input_OnAttackReady();
	void Input_OnAttackReleased();
};