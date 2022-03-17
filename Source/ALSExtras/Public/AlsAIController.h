#pragma once

#include "AIController.h"
#include "AlsAIController.generated.h"

UCLASS(DisplayName = "Als AI Controller")
class ALSEXTRAS_API AAlsAIController : public AAIController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviourTree;

public:
	AAlsAIController();

protected:
	virtual void OnPossess(APawn* NewPawn) override;

public:
	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;
};
