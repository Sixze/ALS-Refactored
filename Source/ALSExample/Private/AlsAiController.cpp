#include "AlsAiController.h"

AAlsAiController::AAlsAiController()
{
	bAttachToPawn = true;
}

void AAlsAiController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	RunBehaviorTree(BehaviourTree);
}

FVector AAlsAiController::GetFocalPointOnActor(const AActor* Actor) const
{
	const auto* FocusedPawn{Cast<APawn>(Actor)};
	if (!IsValid(FocusedPawn))
	{
		return Super::GetFocalPointOnActor(Actor);
	}

	return FocusedPawn->GetPawnViewLocation();
}
