// Fill out your copyright notice in the Description page of Project Settings.


#include "AlsPhysicalAnimationComponent.h"

#include "AlsCharacter.h"
#include "AlsAnimationInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Curves/CurveFloat.h"
#include "Engine/Canvas.h"
#include "Utility/AlsGameplayTags.h"
#include "Utility/AlsConstants.h"
#include "Utility/AlsMacros.h"
#include "Utility/AlsUtility.h"
#include "Utility/AlsLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsPhysicalAnimationComponent)

void FAlsPhysicalAnimationCurveValues::Refresh(const AAlsCharacter* Character)
{
	auto AnimInstance{Cast<UAlsAnimationInstance>(Character->GetMesh()->GetAnimInstance())};
	LockLeftArm = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockArmLeftCurveName());
	LockRightArm = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockArmRightCurveName());
	LockLeftHand = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockHandLeftCurveName());
	LockRightHand = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockHandRightCurveName());
	LockLeftLeg = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockLegLeftCurveName());
	LockRightLeg = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockLegRightCurveName());
	LockLeftFoot = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockFootLeftCurveName());
	LockRightFoot = AnimInstance->GetCurveValueClamped01(UAlsConstants::PALockFootRightCurveName());
}

float FAlsPhysicalAnimationCurveValues::GetLockedValue(const FName& BoneName) const
{
	if (BoneName == FName{TEXT("clavicle_l")} || BoneName == FName{TEXT("upperarm_l")} || BoneName == FName{TEXT("lowerarm_l")})
	{
		return LockLeftArm;
	}
	if (BoneName == FName{TEXT("clavicle_r")} || BoneName == FName{TEXT("upperarm_r")} || BoneName == FName{TEXT("lowerarm_r")})
	{
		return LockRightArm;
	}
	if (BoneName == FName{TEXT("hand_l")})
	{
		return LockLeftHand;
	}
	if (BoneName == FName{TEXT("hand_r")})
	{
		return LockRightHand;
	}
	if (BoneName == FName{TEXT("thigh_l")} || BoneName == FName{TEXT("calf_l")})
	{
		return LockLeftLeg;
	}
	if (BoneName == FName{TEXT("thigh_r")} || BoneName == FName{TEXT("calf_r")})
	{
		return LockRightLeg;
	}
	if (BoneName == UAlsConstants::FootLeftBoneName() || BoneName == FName{TEXT("ball_l")})
	{
		return LockLeftFoot;
	}
	if (BoneName == UAlsConstants::FootRightBoneName() || BoneName == FName{TEXT("ball_r")})
	{
		return LockRightFoot;
	}
	return 0.0f;
}

bool UAlsPhysicalAnimationComponent::IsProfileExist(const FName& ProfileName) const
{
	for (auto Body : GetSkeletalMesh()->Bodies)
	{
		if (USkeletalBodySetup* BodySetup = Cast<USkeletalBodySetup>(Body->BodySetup.Get()))
		{
			if (BodySetup->FindPhysicalAnimationProfile(ProfileName))
			{
				return true;
			}
		}
	}
	return false;
}

bool UAlsPhysicalAnimationComponent::HasAnyProfile(const USkeletalBodySetup* BodySetup) const
{
	if (CurrentProfileNames.IsEmpty())
	{
		return bRagdolling && BodySetup->PhysicsType != EPhysicsType::PhysType_Kinematic;
	}

	for (const auto& ProfileName : CurrentProfileNames)
	{
		if (BodySetup->FindPhysicalAnimationProfile(ProfileName))
		{
			return true;
		}
	}

	return false;
}

bool UAlsPhysicalAnimationComponent::NeedsProfileChange()
{
	bool bRetVal = false;
	if (LocomotionAction != PreviousLocomotionAction)
	{
		bRetVal = true;
	}
	else if (LocomotionMode != PreviousLocomotionMode)
	{
		bRetVal = true;
	}
	else if (Stance != PreviousStance)
	{
		bRetVal = true;
	}
	else if (Gait != PreviousGait)
	{
		bRetVal = true;
	}
	else if (OverlayMode != PreviousOverlayMode)
	{
		bRetVal = true;
	}
	PreviousLocomotionAction = LocomotionAction;
	PreviousLocomotionMode = LocomotionMode;
	PreviousStance = Stance;
	PreviousGait = Gait;
	PreviousOverlayMode = OverlayMode;
	if (CurrentMultiplyProfileNames != MultiplyProfileNames)
	{
		CurrentMultiplyProfileNames = MultiplyProfileNames;
		bRetVal = true;
	}
	return bRetVal;
}

void UAlsPhysicalAnimationComponent::ClearGameplayTags()
{
	LocomotionAction = FGameplayTag::EmptyTag;
	LocomotionMode = FGameplayTag::EmptyTag;
	Stance = FGameplayTag::EmptyTag;
	Gait = FGameplayTag::EmptyTag;
	OverlayMode = FGameplayTag::EmptyTag;
	PreviousLocomotionAction = FGameplayTag::EmptyTag;
	PreviousLocomotionMode = FGameplayTag::EmptyTag;
	PreviousStance = FGameplayTag::EmptyTag;
	PreviousGait = FGameplayTag::EmptyTag;
	PreviousOverlayMode = FGameplayTag::EmptyTag;
}

void UAlsPhysicalAnimationComponent::RefreshBodyState(float DeltaTime)
{
	USkeletalMeshComponent* Mesh{GetSkeletalMesh()};

	bool bNeedUpdate = bActive;

	if (!bActive && (!CurrentProfileNames.IsEmpty() || bRagdolling))
	{
		for (auto Body : Mesh->Bodies)
		{
			if (USkeletalBodySetup* BodySetup = Cast<USkeletalBodySetup>(Body->BodySetup.Get()))
			{
				if (CurveValues.GetLockedValue(BodySetup->BoneName) <= 0.0f && HasAnyProfile(BodySetup))
				{
					bNeedUpdate = true;
					break;
				}
			}
		}
	}

	bool bActiveAny = false;

	if (bNeedUpdate)
	{
		for (auto Body : Mesh->Bodies)
		{
			if (USkeletalBodySetup* BodySetup = Cast<USkeletalBodySetup>(Body->BodySetup.Get()))
			{
				float LockedValue{CurveValues.GetLockedValue(BodySetup->BoneName)};
				if (!FAnimWeight::IsRelevant(LockedValue) && HasAnyProfile(BodySetup))
				{
					bActiveAny = true;
					if (Body->IsInstanceSimulatingPhysics())
					{
						float Speed = 1.0f / FMath::Max(0.000001f, BlendTimeOfBlendWeightOnActivate);
						Body->PhysicsBlendWeight = FMath::Min(1.0f, FMath::FInterpConstantTo(Body->PhysicsBlendWeight, 1.0f, DeltaTime, Speed));
					}
					else
					{
						Body->SetInstanceSimulatePhysics(true);
						Body->PhysicsBlendWeight = 0.0f;
					}
				}
				else
				{
					if (FAnimWeight::IsRelevant(LockedValue))
					{
						if (Body->IsInstanceSimulatingPhysics())
						{
							Body->PhysicsBlendWeight = FMath::FInterpConstantTo(Body->PhysicsBlendWeight, 1.0f - LockedValue, DeltaTime, 15.0f);
						}
						else
						{
							if (!FAnimWeight::IsFullWeight(LockedValue))
							{
								Body->SetInstanceSimulatePhysics(true);
							}
							Body->PhysicsBlendWeight = 0.0f;
						}
					}
					else
					{
						float Speed = 1.0f / FMath::Max(0.000001f, BlendTimeOfBlendWeightOnDeactivate);
						Body->PhysicsBlendWeight = FMath::FInterpConstantTo(Body->PhysicsBlendWeight, 0.0f, DeltaTime, Speed);
					}
					if (Body->PhysicsBlendWeight == 0.0f)
					{
						if (Body->IsInstanceSimulatingPhysics())
						{
							Body->SetInstanceSimulatePhysics(false);
						}
					}
					else
					{
						bActiveAny = true;
					}
				}
			}
		}

		// Trick. Calling a no effect method To call skeletal mesh's private method "UpdateEndPhysicsTickRegisteredState" and "UpdateClothTickRegisteredState".
		Mesh->AccumulateAllBodiesBelowPhysicsBlendWeight(NAME_None, 0.0f);
	}
	
	if (bActiveAny && !bActive)
	{
		PrevCollisionObjectType = TEnumAsByte(Mesh->GetCollisionObjectType());
		PrevCollisionEnabled = TEnumAsByte(Mesh->GetCollisionEnabled());
		Mesh->SetCollisionObjectType(ECC_PhysicsBody);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bActive = true;
	}

	if (!bActiveAny && bActive)
	{
		Mesh->SetCollisionObjectType(PrevCollisionObjectType);
		Mesh->SetCollisionEnabled(PrevCollisionEnabled);
		bActive = false;
	}
}

namespace AlsPhysicalAnimationTagCombination
{
	struct FIterator
	{
		const struct FContainer* Container;
		TArray<FName> CurrentNames;
		TArray<int> Indices;

		const TArray<FName>& operator*() const { return CurrentNames; }
		FIterator& operator++();
		bool operator!=(const FIterator& lhs) const { return Container != Container || Indices != lhs.Indices || CurrentNames != lhs.CurrentNames; }
	};

	struct FContainer
	{
		TArray<FName> SourceNames;
		FIterator begin() const { return {this, SourceNames}; }
		FIterator end() const { return {nullptr}; }
	};

	FIterator& FIterator::operator++()
	{
		if (!Container)
		{
			CurrentNames.Reset();
			Indices.Reset();
			return *this;
		}

		auto SourceNum{Container->SourceNames.Num()};

		// ex. SourceNum = 5
		// SourceNum - IndicesNum(1) = 4
		// 11110 0
		// 11101 1
		// 11011 2
		// 10111 3
		// 01111 4
		// 
		// SourceNum - IndicesNum(2) = 3
		// 11100 0 0
		// 11010 0 1
		// 11001 1 1
		// 10110 0 2
		// 10101 1 2
		// 10011 2 2
		// 01110 0 3
		// 01101 1 3
		// 01011 2 3
		// 00111 3 3
		// 
		// SourceNum - IndicesNum(3) = 2
		// 11000 0 0 0
		// 10100 0 0 1
		// 10010 0 1 1
		// 10001 1 1 1
		// 01100 0 0 2
		// 01010 0 1 2
		// 01001 1 1 2
		// 00110 0 2 2
		// 00101 1 2 2
		// 00011 2 2 2
		// 
		// SourceNum - IndicesNum(4) = 1
		// 10000 0 0 0 0
		// 01000 0 0 0 1
		// 00100 0 0 1 1
		// 00010 0 1 1 1
		// 00001 1 1 1 1

		if (Indices.IsEmpty())
		{
			Indices.Push(0);
		}
		else
		{
			bool bNextDepth = false;
			auto IndicesNum{Indices.Num()};
			for (int Index = 0; Index < IndicesNum; ++Index)
			{
				if (Index + 1 < IndicesNum)
				{
					if (Indices[Index] < Indices[Index + 1])
					{
						Indices[Index]++;
						break;
					}
					else
					{
						Indices[Index] = 0;
					}
				}
				else
				{
					if (Indices[Index] < SourceNum - IndicesNum)
					{
						Indices[Index]++;
					}
					else
					{
						Indices[Index] = 0;
						bNextDepth = true;
					}
				}
			}
			if (bNextDepth)
			{
				if(IndicesNum + 1 < SourceNum)
				{
					Indices.Push(0);
				}
				else
				{
					Container = nullptr;
					CurrentNames.Reset();
					Indices.Reset();
					return *this;
				}
			}
		}

		CurrentNames = Container->SourceNames;

		if (SourceNum > 1)
		{
			for (auto Index : Indices)
			{
				CurrentNames.RemoveAt(CurrentNames.Num() - 1 - Index);
			}
		}

		return *this;
	}
}

void UAlsPhysicalAnimationComponent::SelectProfile()
{
	TArray<FName> NextProfileNames;
	FName NextBaseProfileName;
	TArray<FName> NextAdditiveProfileNames;
	TArray<FName> NextMultiplyProfileNames;
	TStringBuilder<256> StringBuilder;
	TStringBuilder<256> AdditionalStringBuilder;

	auto ActionName{UAlsUtility::GetSimpleTagName(LocomotionAction)};
	auto ModeName{UAlsUtility::GetSimpleTagName(LocomotionMode)};
	auto StanceName{UAlsUtility::GetSimpleTagName(Stance)};
	auto GaitName{UAlsUtility::GetSimpleTagName(Gait)};
	auto OverlayModeName{UAlsUtility::GetSimpleTagName(OverlayMode)};

	FName RagdollingModeName{UAlsUtility::GetSimpleTagName(AlsLocomotionActionTags::Ragdolling)};

	AlsPhysicalAnimationTagCombination::FContainer Container;

	if (ActionName.IsValid() && !ActionName.IsNone())
	{
		Container.SourceNames.Add(ActionName);
	}
	if (ModeName.IsValid() && !ModeName.IsNone())
	{
		Container.SourceNames.Add(ModeName);
	}
	if (StanceName.IsValid() && !StanceName.IsNone())
	{
		Container.SourceNames.Add(StanceName);
	}
	if (GaitName.IsValid() && !GaitName.IsNone())
	{
		Container.SourceNames.Add(GaitName);
	}
	if (OverlayModeName.IsValid() && !OverlayModeName.IsNone())
	{
		Container.SourceNames.Add(OverlayModeName);
	}

	for (auto& Names : Container)
	{
		if(bRagdolling && !Names.Contains(RagdollingModeName))
		{
			continue;
		}

		for(auto& Name : Names)
		{
			if (StringBuilder.Len() > 0)
			{
				StringBuilder << TEXT(":");
			}
			StringBuilder << Name;
		}

		UE_LOG(LogAls, VeryVerbose, TEXT("Try Physical Animation Profile '%s'"), *StringBuilder);

		// determin base profile

		if (NextBaseProfileName.IsNone())
		{
			FName ProfileName(StringBuilder, FNAME_Find);
			if (IsProfileExist(ProfileName))
			{
				NextBaseProfileName = ProfileName;
			}
		}

		// add additive profile if exists

		AdditionalStringBuilder << TEXT("+");
		AdditionalStringBuilder.Append(StringBuilder);
		FName AdditiveProfileName(AdditionalStringBuilder, FNAME_Find);
		AdditionalStringBuilder.Reset();
		if (IsProfileExist(AdditiveProfileName))
		{
			NextAdditiveProfileNames.Add(AdditiveProfileName);
		}

		// add multiply profile if exists

		AdditionalStringBuilder << TEXT("*");
		AdditionalStringBuilder.Append(StringBuilder);
		FName MultiplyProfileName(AdditionalStringBuilder, FNAME_Find);
		AdditionalStringBuilder.Reset();
		if (IsProfileExist(MultiplyProfileName))
		{
			NextMultiplyProfileNames.Add(MultiplyProfileName);
		}

		StringBuilder.Reset();
	}

	if (NextBaseProfileName.IsNone() && bRagdolling && IsProfileExist(RagdollingModeName))
	{
		NextBaseProfileName = RagdollingModeName;
	}
	if (NextBaseProfileName.IsNone() && !bRagdolling && IsProfileExist(UAlsConstants::DefaultPAProfileName()))
	{
		NextBaseProfileName = UAlsConstants::DefaultPAProfileName();
	}

	if (NextBaseProfileName.IsNone())
	{
		NextAdditiveProfileNames.Reset();
		NextMultiplyProfileNames.Reset();
	}
	else
	{
		NextProfileNames.Add(NextBaseProfileName);
		NextProfileNames.Append(NextAdditiveProfileNames);
		NextMultiplyProfileNames.Append(MultiplyProfileNames);
	}

	if (NextProfileNames != CurrentProfileNames || NextMultiplyProfileNames != CurrentMultiplyProfileNames)
	{
		bool bFirst = true;
		for (const auto& NextProfileName : NextProfileNames)
		{
			ApplyPhysicalAnimationProfileBelow(NAME_None, NextProfileName);
			GetSkeletalMesh()->SetConstraintProfileForAll(NextProfileName, bFirst);
			bFirst = false;
		}
		CurrentProfileNames = NextProfileNames;

		for (const auto& NextMultiplyProfileName : NextMultiplyProfileNames)
		{
			ApplyPhysicalAnimationProfileBelow(NAME_None, NextMultiplyProfileName);
			GetSkeletalMesh()->SetConstraintProfileForAll(NextMultiplyProfileName);
		}
		CurrentMultiplyProfileNames = NextMultiplyProfileNames;
	}
}

void UAlsPhysicalAnimationComponent::Refresh(const AAlsCharacter* Character)
{
	// Apply special behaviour when changed Ragdolling state

	if (Character->GetLocomotionAction() == AlsLocomotionActionTags::Ragdolling)
	{
		if (!bRagdolling)
		{
			bRagdolling = true;
			GetSkeletalMesh()->SetAllBodiesBelowSimulatePhysics(UAlsConstants::PelvisBoneName(), true);
			GetSkeletalMesh()->SetAllBodiesPhysicsBlendWeight(1.0f);
			ApplyPhysicalAnimationProfileBelow(NAME_None, NAME_None, true, true);
		}
		bRagdollingFreezed = Character->GetRagdollingState().bFreezing;
	}
	else
	{
		if (bRagdolling)
		{
			bRagdolling = false;
			CurrentProfileNames.Reset();
			CurrentMultiplyProfileNames.Reset();
			ClearGameplayTags();
			GetSkeletalMesh()->SetAllBodiesSimulatePhysics(false);
			GetSkeletalMesh()->SetAllBodiesPhysicsBlendWeight(0.0f);
			GetSkeletalMesh()->SetConstraintProfileForAll(NAME_None, true);

			if (bActive)
			{
				GetSkeletalMesh()->SetCollisionObjectType(PrevCollisionObjectType);
				GetSkeletalMesh()->SetCollisionEnabled(PrevCollisionEnabled);
				bActive = false;
			}
			bRagdollingFreezed = false;
		}
	}

	LocomotionAction = Character->GetLocomotionAction();
	LocomotionMode = Character->GetLocomotionMode();
	Stance = Character->GetStance();
	Gait = Character->GetGait();
	OverlayMode = Character->GetOverlayMode();
	CurveValues.Refresh(Character);
}

void UAlsPhysicalAnimationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Choose Physical Animation Profile

	if (OverrideProfileNames.Num() > 0)
	{
		if (CurrentProfileNames != OverrideProfileNames || CurrentMultiplyProfileNames != MultiplyProfileNames)
		{
			bool bFirst = true;
			for (const auto& CurrentProfileName : OverrideProfileNames)
			{
				ApplyPhysicalAnimationProfileBelow(NAME_None, CurrentProfileName);
				GetSkeletalMesh()->SetConstraintProfileForAll(CurrentProfileName, bFirst);
				bFirst = false;
			}
			CurrentProfileNames = OverrideProfileNames;
			ClearGameplayTags();

			for (const auto& MultiplyProfileName : MultiplyProfileNames)
			{
				ApplyPhysicalAnimationProfileBelow(NAME_None, MultiplyProfileName);
				GetSkeletalMesh()->SetConstraintProfileForAll(MultiplyProfileName);
			}
			CurrentMultiplyProfileNames = MultiplyProfileNames;
		}
	}
	else if(NeedsProfileChange())
	{
		SelectProfile();
	}

	// Update PhysicsBlendWeight and Collision settings

	if (!bRagdolling || !bRagdollingFreezed)
	{
		RefreshBodyState(DeltaTime);
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAlsPhysicalAnimationComponent::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& HorizontalLocation, float& VerticalLocation)
{
	const auto Scale{FMath::Min(Canvas->SizeX / (1280.0f * Canvas->GetDPIScale()), Canvas->SizeY / (720.0f * Canvas->GetDPIScale()))};

	VerticalLocation += 4.0f * Scale;

	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{145.0f * Scale};
	
	TStringBuilder<256> DebugStringBuilder;

	for (const auto& ProfileName : CurrentProfileNames)
	{
		DebugStringBuilder.Appendf(TEXT("%s "), *ProfileName.ToString());
	}

	for (const auto& ProfileName : CurrentMultiplyProfileNames)
	{
		DebugStringBuilder.Appendf(TEXT("%s "), *ProfileName.ToString());
	}

	Text.Text = FText::AsCultureInvariant(DebugStringBuilder.ToString());
	Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

	VerticalLocation += RowOffset;

	for (auto Body : GetSkeletalMesh()->Bodies)
	{
		Text.SetColor(FMath::Lerp(FLinearColor::Gray, FLinearColor::Red, UAlsMath::Clamp01(Body->PhysicsBlendWeight)));

		Text.Text = FText::AsCultureInvariant(FString::Printf(TEXT("%s %s %1.2f"), *Body->GetBodySetup()->BoneName.ToString(),
			Body->IsInstanceSimulatingPhysics() ? TEXT("ON") : TEXT("OFF"),
			Body->PhysicsBlendWeight));
		Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

		VerticalLocation += RowOffset;
	}
}
