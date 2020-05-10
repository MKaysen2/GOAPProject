#include "../Public/GOAPActionsComponent.h"
#include "../Public/GOAPController.h"
#include "../Public/GOAPAction.h"
#include "../Public/StateNode.h"
#include "../Public/WorldState.h"
#include "GameFramework/Character.h"


#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerTypes.h"
#include "GameplayDebuggerCategory.h"
#endif

void UGOAPActionsComponent::OnRegister()
{
	Super::OnRegister();
	ActionIdx = 0;
	bPlanComplete = true;
	AIOwner = Cast<AGOAPController>(GetOwner());
}

void UGOAPActionsComponent::RegisterAction(TSubclassOf<UGOAPAction> ActionClass)
{
	//Null if not a valid subclass
	if (!ActionClass.GetDefaultObject())
	{
		return;
	}
	ActionSet.Add(NewObject<UGOAPAction>(this, ActionClass));
}

void UGOAPActionsComponent::RegisterActionSet(const TArray<TSubclassOf<UGOAPAction>>& NewActionSet)
{
	for (auto Action : NewActionSet)
	{
		RegisterAction(Action);
	}
}

void UGOAPActionsComponent::OnActionEnded()
{
	++ActionIdx;

	RunNextAction();
}

void UGOAPActionsComponent::RunNextAction()
{
	if (ActionIdx >= ActionQueue.Num())
	{
		Reset();
		OnPlanCompleted.ExecuteIfBound();
		return;
	}
	CurrentAction = ActionQueue[ActionIdx];

	if (CurrentAction)
	{
		if (!CurrentAction->VerifyContext(AIOwner))
		{
			CurrentAction = nullptr;
			//TODO fire delegate here I think
			return;
		}
		CurrentAction->SetBBTargets(AIOwner, StateQueue[ActionIdx + 1]);
		//Eventually I want animations that aren't forced to interrupt clean themselves up
		CurrentAction->OnActionEnded.BindUObject(this, &UGOAPActionsComponent::OnActionEnded);
		EActionStatus eStatus = CurrentAction->StartAction(AIOwner);
		if (eStatus == EActionStatus::kFailed)
		{
			CurrentAction->OnActionEnded.Unbind();
			OnPlanCompleted.ExecuteIfBound();
		}
	}
}


void UGOAPActionsComponent::StartPlan(TSharedPtr<FStateNode> CurrentNode)
{
	if (!CurrentNode.IsValid())
	{
		return;
	}
	//The starting node should have a nullptr for the parent node+edge
	while (CurrentNode.IsValid() )
	{
		UGOAPAction* Action = CurrentNode->edge();
		if (IsValid(Action))
		{
			FString action_name = Action->GetName();
			QueueAction(Action);
		}
		StateQueue.Emplace(CurrentNode->GetState());
		CurrentNode = CurrentNode->previous();
	}
	RunNextAction();
}

void UGOAPActionsComponent::AbortPlan()
{
	if (CurrentAction && CurrentAction->IsActionRunning())
	{
		CurrentAction->OnActionEnded.Unbind();
		CurrentAction->StopAction(AIOwner);
		
	}
	
	Reset();
	if (AIOwner->IsPlayingMontage())
	{
		ACharacter* Avatar = Cast<ACharacter>(AIOwner->GetPawn());
		if (Avatar)
			Avatar->StopAnimMontage();
	}
	if (AIOwner->IsFollowingAPath())
	{
		AIOwner->StopMovement();
	}
}

void UGOAPActionsComponent::Reset()
{
	bPlanComplete = true;
	ActionIdx = 0;
	CurrentAction = nullptr;
	ActionQueue.Reset();
	ActionSet.Reset();
	StateQueue.Reset();
}

void UGOAPActionsComponent::QueueAction(UGOAPAction* Action)
{
	bPlanComplete = false;
	ActionQueue.Emplace(Action);
}

bool UGOAPActionsComponent::IsActionRunning()
{
	if (!CurrentAction)
		return false;
	return CurrentAction->IsActionRunning();
}


bool UGOAPActionsComponent::IsPlanComplete()
{
	return bPlanComplete;
}

TArray<UGOAPAction*>& UGOAPActionsComponent::GetActionSet()
{
	return ActionSet;
}

#if WITH_GAMEPLAY_DEBUGGER

void UGOAPActionsComponent::DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const
{
	FString CurrentActionName = (CurrentAction) ? CurrentAction->GetName() : TEXT("NONE");
	DebuggerCategory->AddTextLine(FString::Printf(TEXT("Current Action: %s"), *CurrentActionName, ActionIdx));

	int PlanSize = ActionQueue.Num();
	FString PlanInfo;
	int StateSize = 0;
	if (PlanSize > 0)
	{
		StateSize = PlanSize + 1;
		PlanInfo = FString::Printf(TEXT("Step %d of %d "), ActionIdx + 1, PlanSize);
	}
	else
	{
		PlanInfo = FString(TEXT("No plan"));
	}

	DebuggerCategory->AddTextLine(PlanInfo);

	FString StateInfo = FString::Printf(TEXT("Statequeue: %d states (should be %d)"), StateQueue.Num(), StateSize);
	DebuggerCategory->AddTextLine(StateInfo);
}

#endif //WITH_GAMEPLAY_DEBUGGER