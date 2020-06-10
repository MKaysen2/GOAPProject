#include "../Public/GOAPActionsComponent.h"
#include "../Public/GOAPAction.h"
#include "../Public/WorldState.h"
#include "GameFramework/Character.h"
#include "AIController.h"


#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerTypes.h"
#include "GameplayDebuggerCategory.h"
#endif

void UGOAPActionsComponent::OnRegister()
{
	Super::OnRegister();
	ActionIdx = 0;
	AIOwner = Cast<AAIController>(GetOwner());
	AddActionFinishedListener(AIOwner->GetBrainComponent());

}

void UGOAPActionsComponent::RegisterAction(TSubclassOf<UGOAPAction> ActionClass)
{
	//Null if not a valid subclass
	if (!ActionClass.GetDefaultObject())
	{
		return;
	}
	UGOAPAction* NewAction = NewObject<UGOAPAction>(this, ActionClass);
	NewAction->InitAction(AIOwner);
	ActionSet.Add(NewAction);
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

void UGOAPActionsComponent::AddActionFinishedListener(UBrainComponent* Brain)
{
	ActionFinishedHandle = FAIMessageObserver::Create(Brain, UGOAPAction::ActionFinished, FOnAIMessage::CreateUObject(this, &UGOAPActionsComponent::HandleAIMessage));

}

void UGOAPActionsComponent::RemoveListeners()
{
	ActionFinishedHandle.Reset();
}

void UGOAPActionsComponent::HandleAIMessage(UBrainComponent* BrainComp, const FAIMessage& Message)
{
	bool bSuccess = (Message.Status == FAIMessage::Success);
	UE_LOG(LogTemp, Warning, TEXT("Got AI Message"));
	if (bSuccess)
	{
		OnActionSuccess();
	}
	else
	{
		OnActionFailed();
	}
}

void UGOAPActionsComponent::OnActionSuccess()
{
	++ActionIdx;

	RunNextAction();
}

void UGOAPActionsComponent::OnActionFailed()
{
	ClearCurrentPlan();
	OnPlanCompleted.ExecuteIfBound();
}

void UGOAPActionsComponent::RunNextAction()
{
	if (ActionIdx >= ActionQueue.Num())
	{
		ClearCurrentPlan();
		OnPlanCompleted.ExecuteIfBound();
		return;
	}
	CurrentAction = ActionQueue[ActionIdx];

	if (CurrentAction)
	{
		if (!CurrentAction->VerifyContext())
		{
			CurrentAction = nullptr;
			OnActionFailed();
			return;
		}
		EActionResult eStatus = CurrentAction->StartAction();
		if (eStatus == EActionResult::Failed)
		{
			OnActionFailed();
		}
	}
}

void UGOAPActionsComponent::StartPlan(TArray<UGOAPAction*>& Plan)
{
	for (auto* Action : Plan)
	{
		QueueAction(Action);
	}
	RunNextAction();
}

void UGOAPActionsComponent::AbortPlan()
{
	if (CurrentAction)
	{
		CurrentAction->AbortAction();
	}
	
	ClearCurrentPlan();
}

void UGOAPActionsComponent::ClearCurrentPlan()
{
	ActionIdx = 0;
	CurrentAction = nullptr;
	ActionQueue.Reset();
}

void UGOAPActionsComponent::Reset()
{
	ClearCurrentPlan();
	ActionSet.Reset(); //WHY ARE MY POINTERS DYING IF THE ACTIONSET ISN'T BEING MODIFIED
	//oh yeah I wonder why
}

void UGOAPActionsComponent::QueueAction(UGOAPAction* Action)
{
	ActionQueue.Emplace(Action);
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
}

#endif //WITH_GAMEPLAY_DEBUGGER