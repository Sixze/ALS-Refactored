#include "Notifies/AlsAnimNotify_CameraShake.h"

#include "Camera/CameraShakeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

UAlsAnimNotify_CameraShake::UAlsAnimNotify_CameraShake()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UAlsAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Camera Shake: {0}"), {IsValid(CameraShakeClass) ? CameraShakeClass->GetName() : TEXT("None")});
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
