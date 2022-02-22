#include "Notify/AlsAnimNotify_CameraShake.h"

#include "Camera/CameraShakeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

FString UAlsAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("Als Camera Shake: {0}"), {IsValid(CameraShakeClass) ? CameraShakeClass->GetName() : TEXT("None")});
}

void UAlsAnimNotify_CameraShake::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation)
{
	Super::Notify(Mesh, Animation);

	const auto* Pawn{Cast<APawn>(Mesh->GetOwner())};
	if (!IsValid(Pawn))
	{
		return;
	}

	const auto* Player{Cast<APlayerController>(Pawn->GetController())};
	if (!IsValid(Player))
	{
		return;
	}

	if (IsValid(Player->PlayerCameraManager))
	{
		Player->PlayerCameraManager->StartCameraShake(CameraShakeClass, CameraShakeScale);
	}
}
