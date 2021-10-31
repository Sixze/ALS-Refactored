// Fill out your copyright notice in the Description page of Project Settings.


#include "AlsAttachmentComponent.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAlsAttachmentComponent::UAlsAttachmentComponent()
{
	
}


// Called when the game starts
void UAlsAttachmentComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAttachToCharacterMesh)
	{
		if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			ComponentToAttach = Character->GetMesh();
		}
	}
	
}

void UAlsAttachmentComponent::SetComponentToAttach(USceneComponent* Component)
{
	ComponentToAttach = Component;
}

USceneComponent* UAlsAttachmentComponent::GetComponentToAttach() const
{
	return ComponentToAttach;
}

void UAlsAttachmentComponent::AddStaticMeshAttachment(const FName Socket, UStaticMesh* StaticMesh, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule, const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies)
{
	if (!StaticMesh || Socket.IsNone())
	{
		return;
	}
	
	FAlsAttachment Attachment;
	if (Attachments.Contains(Socket))
	{
		//Attachments.RemoveAndCopyValue(Socket,Attachment);
		Attachment = Attachments.FindAndRemoveChecked(Socket);
	}
	
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this);
	StaticMeshComponent->CreationMethod = EComponentCreationMethod::Native;
	StaticMeshComponent->AttachToComponent(ComponentToAttach,FAttachmentTransformRules(LocationRule, RotationRule, ScaleRule, bWeldSimulatedBodies),Socket);
	StaticMeshComponent->RegisterComponent();

	StaticMeshComponent->SetStaticMesh(StaticMesh);

	switch (LocationRule)
	{
	case EAttachmentRule::KeepRelative:
		StaticMeshComponent->SetRelativeLocation(Transform.GetLocation());
		break;
	case EAttachmentRule::KeepWorld:
		StaticMeshComponent->SetWorldLocation(Transform.GetLocation());
		break;
	case EAttachmentRule::SnapToTarget:
		StaticMeshComponent->SetRelativeLocation(Transform.GetLocation());
		break;
	default: ;
	}

	switch (RotationRule)
	{
	case EAttachmentRule::KeepRelative:
		StaticMeshComponent->SetRelativeRotation(Transform.GetRotation());
		break;
	case EAttachmentRule::KeepWorld:
		StaticMeshComponent->SetWorldRotation(Transform.GetRotation());
		break;
	case EAttachmentRule::SnapToTarget:
		StaticMeshComponent->SetRelativeRotation(Transform.GetRotation());
		break;
	default: ;
	}

	switch (ScaleRule)
	{
	case EAttachmentRule::KeepRelative:
		StaticMeshComponent->SetRelativeScale3D(Transform.GetScale3D());
		break;
	case EAttachmentRule::KeepWorld:
		StaticMeshComponent->SetWorldScale3D(Transform.GetScale3D());
		break;
	case EAttachmentRule::SnapToTarget:
		StaticMeshComponent->SetRelativeScale3D(Transform.GetScale3D());
		break;
	default: ;
	}

	Attachment.StaticMeshComponents.Add(StaticMeshComponent);
	
	Attachments.Add(Socket,Attachment);
}

void UAlsAttachmentComponent::AddSkeletalMeshAttachment(const FName Socket,USkeletalMesh* SkeletalMesh ,TSubclassOf<UAnimInstance> AnimationClass, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule, const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies)
{
	if (!SkeletalMesh || Socket.IsNone())
	{
		return;
	}
	
	FAlsAttachment Attachment;
	if (Attachments.Contains(Socket))
	{
		Attachment = Attachments.FindAndRemoveChecked(Socket);
	}
	
	USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this);
	SkeletalMeshComponent->CreationMethod = EComponentCreationMethod::Native;
	SkeletalMeshComponent->AttachToComponent(ComponentToAttach,FAttachmentTransformRules(LocationRule, RotationRule, ScaleRule, bWeldSimulatedBodies),Socket);
	SkeletalMeshComponent->RegisterComponent();

	SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
	SkeletalMeshComponent->SetAnimInstanceClass(AnimationClass);

	switch (LocationRule)
	{
	case EAttachmentRule::KeepRelative:
		SkeletalMeshComponent->SetRelativeLocation(Transform.GetLocation());
		break;
	case EAttachmentRule::KeepWorld:
		SkeletalMeshComponent->SetWorldLocation(Transform.GetLocation());
		break;
	case EAttachmentRule::SnapToTarget:
		SkeletalMeshComponent->SetRelativeLocation(Transform.GetLocation());
		break;
	default: ;
	}

	switch (RotationRule)
	{
	case EAttachmentRule::KeepRelative:
		SkeletalMeshComponent->SetRelativeRotation(Transform.GetRotation());
		break;
	case EAttachmentRule::KeepWorld:
		SkeletalMeshComponent->SetWorldRotation(Transform.GetRotation());
		break;
	case EAttachmentRule::SnapToTarget:
		SkeletalMeshComponent->SetRelativeRotation(Transform.GetRotation());
		break;
	default: ;
	}

	switch (ScaleRule)
	{
	case EAttachmentRule::KeepRelative:
		SkeletalMeshComponent->SetRelativeScale3D(Transform.GetScale3D());
		break;
	case EAttachmentRule::KeepWorld:
		SkeletalMeshComponent->SetWorldScale3D(Transform.GetScale3D());
		break;
	case EAttachmentRule::SnapToTarget:
		SkeletalMeshComponent->SetRelativeScale3D(Transform.GetScale3D());
		break;
	default: ;
	}

	Attachment.SkeletalMeshComponents.Add(SkeletalMeshComponent);
	
	Attachments.Add(Socket,Attachment);
}

void UAlsAttachmentComponent::AddActorAttachment(const FName Socket, AActor* Actor, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule,const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies)
{
	if (!Actor || Socket.IsNone())
	{
		return;
	}
	
	FAlsAttachment Attachment;
	if (Attachments.Contains(Socket))
	{
		Attachment = Attachments.FindAndRemoveChecked(Socket);
	}

	Actor->AttachToComponent(ComponentToAttach,FAttachmentTransformRules(LocationRule, RotationRule, ScaleRule, bWeldSimulatedBodies),Socket);

	switch (LocationRule)
	{
	case EAttachmentRule::KeepRelative:
		Actor->SetActorRelativeLocation(Transform.GetLocation());
		break;
	case EAttachmentRule::KeepWorld:
		Actor->SetActorLocation(Transform.GetLocation());
		break;
	case EAttachmentRule::SnapToTarget:
		Actor->SetActorRelativeLocation(Transform.GetLocation());
		break;
	default: ;
	}

	switch (RotationRule)
	{
	case EAttachmentRule::KeepRelative:
		Actor->SetActorRelativeRotation(Transform.GetRotation());
		break;
	case EAttachmentRule::KeepWorld:
		Actor->SetActorRotation(Transform.GetRotation());
		break;
	case EAttachmentRule::SnapToTarget:
		Actor->SetActorRelativeRotation(Transform.GetRotation());
		break;
	default: ;
	}

	switch (ScaleRule)
	{
	case EAttachmentRule::KeepRelative:
		Actor->SetActorRelativeScale3D(Transform.GetScale3D());
		break;
	case EAttachmentRule::KeepWorld:
		Actor->SetActorScale3D(Transform.GetScale3D());
		break;
	case EAttachmentRule::SnapToTarget:
		Actor->SetActorRelativeScale3D(Transform.GetScale3D());
		break;
	default: ;
	}

	Attachment.Actors.Add(Actor);
	
	Attachments.Add(Socket,Attachment);
}

void UAlsAttachmentComponent::SpawnAndAddActorAttachment(const FName Socket, const TSubclassOf<AActor> ActorClass, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule,const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies)
{
	if (GetWorld())
	{
		AActor* Actor = GetWorld()->SpawnActor(ActorClass);
		AddActorAttachment(Socket,Actor,Transform,LocationRule,RotationRule,ScaleRule,bWeldSimulatedBodies);
	}
}

void UAlsAttachmentComponent::DestroyStaticMeshes(const FName Socket)
{
	if (Attachments.Contains(Socket))
	{
		for (const auto StaticMeshComponent : Attachments.FindRef(Socket).StaticMeshComponents)
		{
			StaticMeshComponent->UnregisterComponent();
			StaticMeshComponent->DestroyComponent();
		}
		Attachments.FindRef(Socket).StaticMeshComponents.Empty();
	}
}

void UAlsAttachmentComponent::DestroySkeletalMeshes(const FName Socket)
{
	if (Attachments.Contains(Socket))
	{
		for (const auto SkeletalMeshComponent : Attachments.FindRef(Socket).SkeletalMeshComponents)
		{
			SkeletalMeshComponent->UnregisterComponent();
			SkeletalMeshComponent->DestroyComponent();
		}
		Attachments.FindRef(Socket).SkeletalMeshComponents.Empty();
	}
}

void UAlsAttachmentComponent::DestroyActors(const FName Socket)
{
	if (Attachments.Contains(Socket))
	{
		for (const auto Actor : Attachments.FindRef(Socket).Actors)
		{
			Actor->Destroy();
		}
		Attachments.FindRef(Socket).Actors.Empty();
	}
}

void UAlsAttachmentComponent::DestroyAttachments(const FName Socket)
{
	if (Attachments.Contains(Socket))
	{
		DestroyStaticMeshes(Socket);
		DestroySkeletalMeshes(Socket);
		DestroyActors(Socket);
		
		Attachments.Remove(Socket);
	}
}

void UAlsAttachmentComponent::DestroyAllAttachments()
{
	TArray<FName> Sockets;

	Attachments.GetKeys(Sockets);
	
	for(const auto Socket : Sockets)
	{
		DestroyAttachments(Socket);
	}
}

AActor* UAlsAttachmentComponent::DetachActor(const FName Socket, const EDetachmentRule LocationRule, const EDetachmentRule RotationRule, const EDetachmentRule ScaleRule, const bool bCallModify)
{
	AActor* Actor = nullptr;
	if (Attachments.Contains(Socket))
	{
		Actor = Attachments.FindRef(Socket).Actors.Pop();
		if (Actor)
		{
			Actor->DetachFromActor(FDetachmentTransformRules(LocationRule,RotationRule,ScaleRule,bCallModify));
		}
	}
	return Actor;
}

TArray<AActor*> UAlsAttachmentComponent::DetachActors(const FName Socket, const EDetachmentRule LocationRule, const EDetachmentRule RotationRule, const EDetachmentRule ScaleRule, const bool bCallModify)
{
	TArray<AActor*> Actors;
	
	if (Attachments.Contains(Socket))
	{
		for (auto Actor : Attachments.FindRef(Socket).Actors)
		{
			Attachments.FindRef(Socket).Actors.Remove(Actor);
			Actor->DetachFromActor(FDetachmentTransformRules(LocationRule,RotationRule,ScaleRule,bCallModify));

			Actors.Add(Actor);
		}
	}

	return Actors;
}

TArray<UStaticMeshComponent*> UAlsAttachmentComponent::GetStaticMeshComponentsAttachments(const FName Socket)
{
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	
	if (Attachments.Contains(Socket))
	{
		StaticMeshComponents = Attachments.FindChecked(Socket).StaticMeshComponents;
	}
	
	return StaticMeshComponents;
}

TArray<USkeletalMeshComponent*> UAlsAttachmentComponent::GetSkeletalMeshComponentsAttachments(const FName Socket)
{
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	
	if (Attachments.Contains(Socket))
	{
		SkeletalMeshComponents = Attachments.FindChecked(Socket).SkeletalMeshComponents;
	}
	
	return SkeletalMeshComponents;
}

TArray<AActor*> UAlsAttachmentComponent::GetActorsAttachments(const FName Socket)
{
	TArray<AActor*> Actors;
	
	if (Attachments.Contains(Socket))
	{
		Actors = Attachments.FindChecked(Socket).Actors;
	}
	
	return Actors;
}

FAlsAttachment UAlsAttachmentComponent::GetAttachments(const FName Socket)
{
	return Attachments[Socket];
}

TMap<FName, FAlsAttachment> UAlsAttachmentComponent::GetAllAttachments()
{
	return Attachments;
}

