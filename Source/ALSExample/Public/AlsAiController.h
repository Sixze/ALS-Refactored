#pragma once

#include "AIController.h"
#include "AlsAiController.generated.h"

UCLASS()
class ALSEXAMPLE_API AAlsAiController : public AAIController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	UBehaviorTree* BehaviourTree;

public:
	AAlsAiController();

protected:
	virtual void OnPossess(APawn* NewPawn) override;

public:
	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;
};
