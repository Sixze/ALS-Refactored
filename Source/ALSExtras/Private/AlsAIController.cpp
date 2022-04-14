#include "AlsAIController.h"

AAlsAIController::AAlsAIController()
{
	bAttachToPawn = true;
}

void AAlsAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	RunBehaviorTree(BehaviourTree);
}

FVector AAlsAIController::GetFocalPointOnActor(const AActor* Actor) const
{
	const auto* FocusedPawn{Cast<APawn>(Actor)};
	if (IsValid(FocusedPawn))
	{
		return FocusedPawn->GetPawnViewLocation();
	}

	return Super::GetFocalPointOnActor(Actor);
}
