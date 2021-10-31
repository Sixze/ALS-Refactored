// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Settings/AlsAttachment.h"
#include "AlsAttachmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALS_API UAlsAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category="Attachment")
	bool bAttachToCharacterMesh = true;

protected:

	UPROPERTY()
	USceneComponent* ComponentToAttach;

	UPROPERTY()
	TMap<FName,FAlsAttachment> Attachments;

	
private:

public:

	// Sets default values for this component's properties
	UAlsAttachmentComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void SetComponentToAttach(USceneComponent* Component);

	UFUNCTION(BlueprintPure,Category="Attachment")
	USceneComponent* GetComponentToAttach() const;
	
	UFUNCTION(BlueprintCallable,Category="Attachment")
	void AddStaticMeshAttachment(const FName Socket, UStaticMesh* StaticMesh, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule, const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void AddSkeletalMeshAttachment(const FName Socket,USkeletalMesh* SkeletalMesh ,TSubclassOf<UAnimInstance> AnimationClass, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule, const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void AddActorAttachment(const FName Socket,AActor* Actor, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule, const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void SpawnAndAddActorAttachment(const FName Socket, const TSubclassOf<AActor> ActorClass, const FTransform Transform, const EAttachmentRule LocationRule, const EAttachmentRule RotationRule, const EAttachmentRule ScaleRule, const bool bWeldSimulatedBodies);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void DestroyStaticMeshes(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void DestroySkeletalMeshes(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void DestroyActors(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void DestroyAttachments(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	void DestroyAllAttachments();

	UFUNCTION(BlueprintCallable,Category="Attachment")
	AActor* DetachActor(const FName Socket, const EDetachmentRule LocationRule, const EDetachmentRule RotationRule, const EDetachmentRule ScaleRule, const bool bCallModify);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	TArray<AActor*> DetachActors(const FName Socket, const EDetachmentRule LocationRule, const EDetachmentRule RotationRule, const EDetachmentRule ScaleRule, const bool bCallModify);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	TArray<UStaticMeshComponent*> GetStaticMeshComponentsAttachments(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	TArray<USkeletalMeshComponent*> GetSkeletalMeshComponentsAttachments(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	TArray<AActor*> GetActorsAttachments(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	FAlsAttachment GetAttachments(const FName Socket);

	UFUNCTION(BlueprintCallable,Category="Attachment")
	TMap<FName,FAlsAttachment> GetAllAttachments();

protected:

private:
		
};
