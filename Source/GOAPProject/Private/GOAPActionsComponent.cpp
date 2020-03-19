#include "../Public/GOAPActionsComponent.h"
#include "../Public/GOAPController.h"
#include "../Public/GOAPAction.h"
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

void UGOAPActionsComponent::RunNextAction() 
{
	if (ActionIdx >= ActionQueue.Num())
	{
		OnPlanCompleted.ExecuteIfBound();
		return;
	}
	CurrentAction = ActionQueue[ActionIdx];

	++ActionIdx;
	if (CurrentAction)
	{
		if (!CurrentAction->VerifyContext(AIOwner))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Invalid Context"));
			CurrentAction = nullptr;
			//TODO fire delegate here I think
			return;
		}
		//Eventually I want animations that aren't forced to interrupt clean themselves up
		CurrentAction->OnActionEnded.BindUObject(this, &UGOAPActionsComponent::RunNextAction);
		CurrentAction->StartAction(AIOwner);
	}
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
	if (PlanSize > 0)
	{

		PlanInfo = FString::Printf(TEXT("Step %d of %d"), ActionIdx, PlanSize);
	}
	else
	{
		PlanInfo = FString(TEXT("No plan"));
	}

	DebuggerCategory->AddTextLine(PlanInfo);
}

#endif //WITH_GAMEPLAY_DEBUGGER