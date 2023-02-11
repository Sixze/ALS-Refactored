#include "AlsAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AlsAIController)

AAlsAIController::AAlsAIController()
{
	bAttachToPawn = true;
}

void AAlsAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	RunBehaviorTree(BehaviorTree);
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
