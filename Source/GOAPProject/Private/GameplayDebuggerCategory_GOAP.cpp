#include "../Public/GameplayDebuggerCategory_GOAP.h"

#if WITH_GAMEPLAY_DEBUGGER


#include "GameFramework/Pawn.h"
#include "../Public/GOAPController.h"
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
		AGOAPController* GOAPAI = Cast<AGOAPController>(MyPawn->GetController());
		if (GOAPAI)
		{
			UGOAPActionsComponent* ActionsComp = GOAPAI->GetGOAPActionsComponent();

			if (ActionsComp)
			{
				ActionsComp->DescribeSelfToGameplayDebugger(this);
			}

			UGoalSelectionComponent* GoalsComp = GOAPAI->GetGoalSelectionComponent();

			if (GoalsComp)
			{
				GoalsComp->DescribeSelfToGameplayDebugger(this);
			}

			TSharedPtr<FWorldState> WS = GOAPAI->GetWorldState();
			if (WS.IsValid())
			{
				WS->DescribeSelfToGameplayDebugger(this);
			}
		}
	}
}

#endif //WITH_GAMEPLAY_DEBUGGER