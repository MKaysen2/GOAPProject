#include "../Public/GOAPActionsComponent.h"
#include "../Public/GOAPController.h"
#include "../Public/GOAPAction.h"

void UGOAPActionsComponent::OnRegister()
{
	Super::OnRegister();
	ActionIdx = 0;
	AIOwner = Cast<AGOAPController>(GetOwner());
}

void UGOAPActionsComponent::RunNextAction() 
{
	if (ActionIdx >= ActionsQueue.Num())
	{
		return;
	}
	CurrentAction = ActionsQueue[ActionIdx];

	if (CurrentAction)
	{
		bPlanDone = false;
		//Eventually I want animations that aren't forced to interrupt clean themselves up
		CurrentAction->OnActionEnded.BindUObject(this, &UGOAPActionsComponent::RunNextAction);
		CurrentAction->StartAction(AIOwner);
	}
	++ActionIdx;
}

void UGOAPActionsComponent::AbortPlan()
{
	if (CurrentAction && CurrentAction->IsActionRunning())
	{
		CurrentAction->OnActionEnded.Unbind();
		CurrentAction->StopAction(AIOwner);
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
	
	Reset();
}

void UGOAPActionsComponent::Reset()
{
	ActionIdx = 0;
	CurrentAction = nullptr;
	bPlanDone = true;
	ActionsQueue.Reset();
}

void UGOAPActionsComponent::QueueAction(UGOAPAction* Action)
{
	ActionsQueue.Emplace(Action);
}

bool UGOAPActionsComponent::IsActionRunning()
{
	if (!CurrentAction)
		return false;
	return CurrentAction->IsActionRunning();
}

bool UGOAPActionsComponent::IsPlanComplete()
{
	return false;
}