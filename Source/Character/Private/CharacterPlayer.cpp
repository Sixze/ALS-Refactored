// Copyright Lubin Tu, All Rights Reserved.

#include "CharacterPlayer.h"

#include "AlsCameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "GAS/BasicAttributeSet.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterPlayer)

ACharacterPlayer::ACharacterPlayer()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(FName{TEXTVIEW("Camera")});
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});

	// GAS相关
	// 给角色添加能力系统组件
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(FName{TEXTVIEW("AbilitySystemComponent")});
	// 给角色添加属性
	BasicAttributes = CreateDefaultSubobject<UBasicAttributeSet>(FName{TEXTVIEW("StandardAttributes")});
	// BasicAttributes = AbilitySystemComponent->GetSet<UBasicAttributeSet>();
}

// 开始
void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 在 CharacterPlayer 类中的某个方法或函数中
	// 判断当前对象是否具有权限执行该操作
	// 若具有权限，则执行相应的逻辑，否则不执行
	if (HasAuthority())
	{
		InitializeAttributes();
	}
}

void ACharacterPlayer::NotifyControllerChanged()
{
	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* InputSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())
		};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())
		};
		if (IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;

			InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
		}
	}

	Super::NotifyControllerChanged();
}

void ACharacterPlayer::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}

void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLookMouse);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLook);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSprint);
		EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRagdoll);
		EnhancedInput->BindAction(RollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRoll);
		EnhancedInput->BindAction(RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRotationMode);
		EnhancedInput->BindAction(ViewModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnViewMode);
		EnhancedInput->BindAction(SwitchShoulderAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSwitchShoulder);
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAttackReady);
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Completed, this, &ThisClass::Input_OnAttackReleased);
	}
}

void ACharacterPlayer::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

void ACharacterPlayer::Input_OnLook(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpRate);
	AddControllerYawInput(Value.X * LookRightRate);
}

void ACharacterPlayer::Input_OnMove(const FInputActionValue& ActionValue)
{
	const auto Value{UAlsMath::ClampMagnitude012D(ActionValue.Get<FVector2D>())};

	const auto ForwardDirection{UAlsMath::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	const auto RightDirection{UAlsMath::PerpendicularCounterClockwiseXY(ForwardDirection)};

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void ACharacterPlayer::Input_OnSprint(const FInputActionValue& ActionValue)
{
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void ACharacterPlayer::Input_OnWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void ACharacterPlayer::Input_OnCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

void ACharacterPlayer::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (StopRagdolling())
		{
			return;
		}

		if (StartMantlingGrounded())
		{
			return;
		}

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ACharacterPlayer::Input_OnAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

void ACharacterPlayer::Input_OnRagdoll()
{
	if (!StopRagdolling())
	{
		StartRagdolling();
	}
}

void ACharacterPlayer::Input_OnRoll()
{
	static constexpr auto PlayRate{1.3f};

	StartRolling(PlayRate);
}

void ACharacterPlayer::Input_OnRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection ? AlsRotationModeTags::ViewDirection : AlsRotationModeTags::VelocityDirection);
}

void ACharacterPlayer::Input_OnViewMode()
{
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ACharacterPlayer::Input_OnSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void ACharacterPlayer::Input_OnAttackReady()
{
	SetAttackState(AlsAttackStateTags::AttackReady);
}

void ACharacterPlayer::Input_OnAttackReleased()
{
	SetAttackState(AlsAttackStateTags::None);
}

void ACharacterPlayer::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused,
                                    float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}

float ACharacterPlayer::GetHealth() const
{
	if (IsValid(BasicAttributes) == false)
	{
		return 0.0f;
	}
	return BasicAttributes->GetHealth();
}

void ACharacterPlayer::InitializeAttributes()
{
	if (IsValid(AbilitySystemComponent) == false || IsValid(BasicAttributes) == false)
	{
		return;
	}
	
	if (IsValid(InitialGameplayEffect))
	{
		// 初始化玩家角色的能力和效果，包括应用游戏效果和设置生命值属性改变的委托
		AbilitySystemComponent->ApplyGameplayEffectToSelf(InitialGameplayEffect->GetDefaultObject<UGameplayEffect>(), 0.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBasicAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthAttributeChanged);
	}
}


void ACharacterPlayer::OnHealthAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	// 显示当前生命值
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), OnAttributeChangeData.NewValue);
	if (FMath::IsNearlyEqual(OnAttributeChangeData.NewValue, 0.f) && OnAttributeChangeData.OldValue > 0.f)
	{
		if (!StopRagdolling())
		{
			StartRagdolling();
			// 10秒后destroy
			GetWorld()->GetTimerManager().SetTimer(OnDeathTimerHandle, this, &ACharacterPlayer::OnDeath, 10.f, false);
		}
	}
}

// 调用OnDeath函数
void ACharacterPlayer::OnDeath()
{
	Destroy();
}