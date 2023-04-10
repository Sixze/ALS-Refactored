#include "Notifies/AlsAnimNotify_CameraShake.h"

#include "Camera/CameraShakeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAnimNotify_CameraShake)

UAlsAnimNotify_CameraShake::UAlsAnimNotify_CameraShake()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UAlsAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	TStringBuilder<256> NotifyNameBuilder;

	NotifyNameBuilder << TEXTVIEW("Als Camera Shake: ");

	if (IsValid(CameraShakeClass))
	{
		NotifyNameBuilder << CameraShakeClass->GetFName();
	}

	return FString{NotifyNameBuilder};
}

void UAlsAnimNotify_CameraShake::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(Mesh, Animation, EventReference);

	const auto* Pawn{Cast<APawn>(Mesh->GetOwner())};
	const auto* Player{IsValid(Pawn) ? Cast<APlayerController>(Pawn->GetController()) : nullptr};
	auto* CameraManager{IsValid(Player) ? Player->PlayerCameraManager.Get() : nullptr};

	if (IsValid(CameraManager))
	{
		CameraManager->StartCameraShake(CameraShakeClass, CameraShakeScale);
	}
}
