#include "../Public/GameplayDebuggerCategory_GOAP.h"

#if WITH_GAMEPLAY_DEBUGGER


#include "GameFramework/Pawn.h"
#include "../Public/GOAPActionsComponent.h"
#include "../Public/GoalSelectionComponent.h"

FGameplayDebuggerCategory_GOAP::FGameplayDebuggerCategory_GOAP()
{

}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_GOAP::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_GOAP());
}

void FGameplayDebuggerCategory_GOAP::CollectData(APlayerController* OwnerPC, AActor* DebugActor) 
{
	APawn* MyPawn = Cast<APawn>(DebugActor);
	if (MyPawn)
	{
		UGOAPActionsComponent* ActionsComp = Cast<UGOAPActionsComponent>(MyPawn->GetController()->GetComponentByClass(UGOAPActionsComponent::StaticClass()));
		if (ActionsComp)
		{
			ActionsComp->DescribeSelfToGameplayDebugger(this);
		}

		UGoalSelectionComponent* GoalsComp = Cast<UGoalSelectionComponent>(MyPawn->GetController()->GetComponentByClass(UGoalSelectionComponent::StaticClass()));

		if (GoalsComp)
		{
			GoalsComp->DescribeSelfToGameplayDebugger(this);
		}

	}
}

#endif //WITH_GAMEPLAY_DEBUGGER