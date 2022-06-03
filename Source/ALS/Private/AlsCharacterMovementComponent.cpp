#include "AlsCharacterMovementComponent.h"

#include "AlsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Controller.h"

void FAlsCharacterNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& Move, const ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(Move, MoveType);

	const auto& SavedMove{static_cast<const FAlsSavedMove&>(Move)};

	Stance = SavedMove.Stance;
	RotationMode = SavedMove.RotationMode;
	MaxAllowedGait = SavedMove.MaxAllowedGait;
}

bool FAlsCharacterNetworkMoveData::Serialize(UCharacterMovementComponent& Movement, FArchive& Archive,
                                             UPackageMap* Map, const ENetworkMoveType MoveType)
{
	Super::Serialize(Movement, Archive, Map, MoveType);

	SerializeOptionalValue(Archive.IsSaving(), Archive, Stance, EAlsStance::Standing);
	SerializeOptionalValue(Archive.IsSaving(), Archive, RotationMode, EAlsRotationMode::LookingDirection);
	SerializeOptionalValue(Archive.IsSaving(), Archive, MaxAllowedGait, EAlsGait::Walking);

	return !Archive.IsError();
}

FAlsCharacterNetworkMoveDataContainer::FAlsCharacterNetworkMoveDataContainer()
{
	NewMoveData = &MoveData[0];
	PendingMoveData = &MoveData[1];
	OldMoveData = &MoveData[2];
}

void FAlsSavedMove::Clear()
{
	Super::Clear();

	Stance = EAlsStance::Standing;
	RotationMode = EAlsRotationMode::LookingDirection;
	MaxAllowedGait = EAlsGait::Walking;
}

void FAlsSavedMove::SetMoveFor(ACharacter* Character, const float NewDeltaTime, const FVector& NewAcceleration,
                               FNetworkPredictionData_Client_Character& PredictionData)
{
	Super::SetMoveFor(Character, NewDeltaTime, NewAcceleration, PredictionData);

	const auto* Movement{Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement())};
	if (IsValid(Movement))
	{
		Stance = Movement->Stance;
		RotationMode = Movement->RotationMode;
		MaxAllowedGait = Movement->MaxAllowedGait;
	}
}

bool FAlsSavedMove::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, const float MaxDelta) const
{
	const auto* NewMove{static_cast<FAlsSavedMove*>(NewMovePtr.Get())};

	return Stance == NewMove->Stance &&
	       RotationMode == NewMove->RotationMode &&
	       MaxAllowedGait == NewMove->MaxAllowedGait &&
	       Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}

void FAlsSavedMove::CombineWith(const FSavedMove_Character* PreviousMove, ACharacter* Character,
                                APlayerController* PlayerController, const FVector& PreviousStartLocation)
{
	const auto* Movement{Character->GetCharacterMovement()};
	const auto InitialRotation{Movement->UpdatedComponent->GetComponentRotation()};

	FSavedMove_Character::CombineWith(PreviousMove, Character, PlayerController, PreviousStartLocation);

	// Restore initial rotation after movement combining. Without this, any rotation applied in
	// the character class will be discarded and the character will not be able to rotate properly.

	Movement->UpdatedComponent->SetWorldRotation(InitialRotation, false, nullptr, Movement->GetTeleportType());
}

void FAlsSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	auto* Movement{Cast<UAlsCharacterMovementComponent>(Character->GetCharacterMovement())};
	if (IsValid(Movement))
	{
		Movement->Stance = Stance;
		Movement->RotationMode = RotationMode;
		Movement->MaxAllowedGait = MaxAllowedGait;

		Movement->RefreshGaitSettings();
	}
}

FAlsNetworkPredictionData::FAlsNetworkPredictionData(const UCharacterMovementComponent& Movement) : Super(Movement) {}

FSavedMovePtr FAlsNetworkPredictionData::AllocateNewMove()
{
	return MakeShared<FAlsSavedMove>();
}

UAlsCharacterMovementComponent::UAlsCharacterMovementComponent()
{
	SetNetworkMoveDataContainer(MoveDataContainer);

	// NetworkMaxSmoothUpdateDistance = 92.0f;
	// NetworkNoSmoothUpdateDistance = 140.0f;

	MaxAcceleration = 1500.0f;
	BrakingFrictionFactor = 0.0f;
	SetCrouchedHalfHeight(56.0f);

	bRunPhysicsWithNoController = true;

	GroundFriction = 4.0f;
	MaxWalkSpeed = 375.0f;
	MaxWalkSpeedCrouched = 200.0f;
	MinAnalogWalkSpeed = 25.0f;
	bCanWalkOffLedgesWhenCrouching = true;
	bIgnoreBaseRotation = true;

	PerchRadiusThreshold = 20.0f;
	PerchAdditionalHeight = 0.0f;
	LedgeCheckThreshold = 0.0f;

	AirControl = 0.15f;

	// https://unrealengine.hatenablog.com/entry/2019/01/16/231404

	FallingLateralFriction = 1.0f;
	JumpOffJumpZFactor = 0.0f;

	bNetworkAlwaysReplicateTransformUpdateTimestamp = true; // Required for view interpolation.

	RotationRate = FRotator::ZeroRotator;
	bUseControllerDesiredRotation = false;
	bOrientRotationToMovement = false;

	bAllowPhysicsRotationDuringAnimRootMotion = true; // Used to allow character rotation while rolling.

	NavAgentProps.bCanCrouch = true;
	NavAgentProps.bCanFly = true;
	bUseAccelerationForPaths = true;
}

#if WITH_EDITOR
bool UAlsCharacterMovementComponent::CanEditChange(const FProperty* Property) const
{
	auto bCanEditChange{Super::CanEditChange(Property)};

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, bIgnoreBaseRotation) ||
	    Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, RotationRate) ||
	    Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, bUseControllerDesiredRotation) ||
	    Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, bOrientRotationToMovement))
	{
		bCanEditChange = false;
	}

	return bCanEditChange;
}

#endif

void UAlsCharacterMovementComponent::BeginPlay()
{
	ensureMsgf(bIgnoreBaseRotation, TEXT("Non-ignored base rotation is not supported."));

	ensureMsgf(!bUseControllerDesiredRotation && !bOrientRotationToMovement,
	           TEXT("These settings are not allowed and must be turned off!"));

	Super::BeginPlay();
}

void UAlsCharacterMovementComponent::SetMovementMode(const EMovementMode NewMovementMode, const uint8 NewCustomMode)
{
	if (!bMovementModeLocked)
	{
		Super::SetMovementMode(NewMovementMode, NewCustomMode);
	}
}

void UAlsCharacterMovementComponent::OnMovementModeChanged(const EMovementMode PreviousMovementMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	// This removes some very noticeable changes in the mesh location when the
	// character automatically uncrouches at the end of the roll in the air.

	bCrouchMaintainsBaseLocation = true;
}

float UAlsCharacterMovementComponent::GetMaxAcceleration() const
{
	// Get the acceleration using the movement curve. This allows for fine control over movement behavior at each speed.

	return IsMovingOnGround() && ensure(!GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve.IsNull())
		       ? GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->FloatCurves[0].Eval(CalculateGaitAmount())
		       : Super::GetMaxAcceleration();
}

float UAlsCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	// Get the deceleration using the movement curve. This allows for fine control over movement behavior at each speed.

	return IsMovingOnGround() && ensure(!GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve.IsNull())
		       ? GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->FloatCurves[1].Eval(CalculateGaitAmount())
		       : Super::GetMaxBrakingDeceleration();
}

void UAlsCharacterMovementComponent::ControlledCharacterMove(const FVector& InputVector, const float DeltaTime)
{
	Super::ControlledCharacterMove(InputVector, DeltaTime);

	const auto* Controller{CharacterOwner->GetController()};
	if (IsValid(Controller))
	{
		PreviousControlRotation = Controller->GetControlRotation();
	}
}

void UAlsCharacterMovementComponent::PhysicsRotation(const float DeltaTime)
{
	Super::PhysicsRotation(DeltaTime);

	if (HasValidData() && (bRunPhysicsWithNoController || !CharacterOwner->Controller.IsNull()))
	{
		OnPhysicsRotation.Broadcast(DeltaTime);
	}
}

void UAlsCharacterMovementComponent::PhysWalking(const float DeltaTime, const int32 Iterations)
{
	if (ensure(!GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve.IsNull()))
	{
		// Get the ground friction using the movement curve. This allows for fine control over movement behavior at each speed.

		GroundFriction = GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->FloatCurves[2].Eval(CalculateGaitAmount());
	}

	Super::PhysWalking(DeltaTime, Iterations);
}

void UAlsCharacterMovementComponent::PhysNavWalking(const float DeltaTime, const int32 Iterations)
{
	if (ensure(!GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve.IsNull()))
	{
		// Get the ground friction using the movement curve. This allows for fine control over movement behavior at each speed.

		GroundFriction = GaitSettings.AccelerationAndDecelerationAndGroundFrictionCurve->FloatCurves[2].Eval(CalculateGaitAmount());
	}

	Super::PhysNavWalking(DeltaTime, Iterations);
}

void UAlsCharacterMovementComponent::PhysCustom(const float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		Super::PhysCustom(DeltaTime, Iterations);
		return;
	}

	Iterations += 1;
	bJustTeleported = false;

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = FVector::ZeroVector;
	}

	ApplyRootMotionToVelocity(DeltaTime);

	MoveUpdatedComponent(Velocity * DeltaTime, UpdatedComponent->GetComponentQuat(), false);

	Super::PhysCustom(DeltaTime, Iterations);
}

void UAlsCharacterMovementComponent::PerformMovement(const float DeltaTime)
{
	Super::PerformMovement(DeltaTime);

	// Update the ServerLastTransformUpdateTimeStamp when the control rotation
	// changes. This is required for the view network smoothing to work properly.

	const auto* Controller{HasValidData() ? CharacterOwner->GetController() : nullptr};

	if (IsValid(Controller) && CharacterOwner->GetLocalRole() >= ROLE_Authority &&
	    PreviousControlRotation != Controller->GetControlRotation())
	{
		if (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy)
		{
			ServerLastTransformUpdateTimeStamp = GetPredictionData_Server_Character()->ServerAccumulatedClientTimeStamp;
		}
		else
		{
			ServerLastTransformUpdateTimeStamp = GetWorld()->GetTimeSeconds();
		}
	}
}

FNetworkPredictionData_Client* UAlsCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		auto* MutableThis{const_cast<UAlsCharacterMovementComponent*>(this)};

		MutableThis->ClientPredictionData = new FAlsNetworkPredictionData{*this};
	}

	return ClientPredictionData;
}

void UAlsCharacterMovementComponent::SmoothClientPosition(const float DeltaTime)
{
	auto* Mesh{HasValidData() ? CharacterOwner->GetMesh() : nullptr};

	if (NetworkSmoothingMode == ENetworkSmoothingMode::Disabled || !IsValid(Mesh) ||
	    !Mesh->IsUsingAbsoluteRotation() || Mesh->IsSimulatingPhysics())
	{
		Super::SmoothClientPosition(DeltaTime);
		return;
	}

	// Ignore mesh rotation smoothing when using absolute mesh rotation because in this case ALS controls the mesh rotation itself.

	const auto InitialRotation{Mesh->GetComponentQuat()};

	Super::SmoothClientPosition(DeltaTime);

	Mesh->SetWorldRotation(InitialRotation);
}

void UAlsCharacterMovementComponent::MoveAutonomous(const float ClientTimeStamp, const float DeltaTime,
                                                    const uint8 CompressedFlags, const FVector& NewAcceleration)
{
	const auto* MoveData{static_cast<FAlsCharacterNetworkMoveData*>(GetCurrentNetworkMoveData())};
	if (MoveData != nullptr)
	{
		Stance = MoveData->Stance;
		RotationMode = MoveData->RotationMode;
		MaxAllowedGait = MoveData->MaxAllowedGait;

		RefreshGaitSettings();
	}

	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAcceleration);

	// Process view network smoothing on the listen server.

	const auto* Controller{HasValidData() ? CharacterOwner->GetController() : nullptr};

	if (IsValid(Controller) && IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy)
	{
		const auto NewControlRotation{Controller->GetControlRotation()};

		auto* Character{Cast<AAlsCharacter>(CharacterOwner)};
		if (IsValid(Character))
		{
			Character->CorrectViewNetworkSmoothing(NewControlRotation);
		}

		PreviousControlRotation = NewControlRotation;
	}
}

void UAlsCharacterMovementComponent::SetMovementSettings(UAlsMovementSettings* NewMovementSettings)
{
	ensure(IsValid(NewMovementSettings));

	MovementSettings = NewMovementSettings;

	RefreshGaitSettings();
}

void UAlsCharacterMovementComponent::RefreshGaitSettings()
{
	if (ensure(!MovementSettings.IsNull()))
	{
		GaitSettings = *MovementSettings->GetMovementStanceSettingsForRotationMode(RotationMode)
		                                ->GetMovementGaitSettingsForStance(Stance);
	}

	RefreshMaxWalkSpeed();
}

void UAlsCharacterMovementComponent::SetStance(const EAlsStance NewStance)
{
	if (Stance != NewStance)
	{
		Stance = NewStance;

		RefreshGaitSettings();
	}
}

void UAlsCharacterMovementComponent::SetRotationMode(const EAlsRotationMode NewMode)
{
	if (RotationMode != NewMode)
	{
		RotationMode = NewMode;

		RefreshGaitSettings();
	}
}

void UAlsCharacterMovementComponent::SetMaxAllowedGait(const EAlsGait NewGait)
{
	if (MaxAllowedGait != NewGait)
	{
		MaxAllowedGait = NewGait;

		RefreshMaxWalkSpeed();
	}
}

void UAlsCharacterMovementComponent::RefreshMaxWalkSpeed()
{
	MaxWalkSpeed = GaitSettings.GetSpeedForGait(MaxAllowedGait);
	MaxWalkSpeedCrouched = MaxWalkSpeed;
}

float UAlsCharacterMovementComponent::CalculateGaitAmount() const
{
	// Map the character's current speed to the configured movement speeds ranging from 0 to 3,
	// where 0 is stopped, 1 is walking, 2 is running and 3 is sprinting. This allows us to vary
	// the movement speeds but still use the mapped range in calculations for consistent results.

	const auto Speed{UE_REAL_TO_FLOAT(Velocity.Size2D())};

	if (Speed <= GaitSettings.WalkSpeed)
	{
		static const FVector2f GaitAmount{0.0f, 1.0f};

		return FMath::GetMappedRangeValueClamped({0.0f, GaitSettings.WalkSpeed}, GaitAmount, Speed);
	}

	if (Speed <= GaitSettings.RunSpeed)
	{
		static const FVector2f GaitAmount{1.0f, 2.0f};

		return FMath::GetMappedRangeValueClamped({GaitSettings.WalkSpeed, GaitSettings.RunSpeed}, GaitAmount, Speed);
	}

	static const FVector2f GaitAmount{2.0f, 3.0f};

	return FMath::GetMappedRangeValueClamped({GaitSettings.RunSpeed, GaitSettings.SprintSpeed}, GaitAmount, Speed);
}

void UAlsCharacterMovementComponent::SetMovementModeLocked(const bool bNewMovementModeLocked)
{
	bMovementModeLocked = bNewMovementModeLocked;
}
