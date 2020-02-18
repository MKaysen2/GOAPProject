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
	
	//this would only matter on multithreading
	if (CurrentAction && CurrentAction->IsActionRunning())
		CurrentAction->stop_action(AIOwner);

	if (ActionIdx >= ActionsQueue.Num())
		return;
	CurrentAction = ActionsQueue[ActionIdx];

	if (CurrentAction)
		CurrentAction->start_action(AIOwner);
	++ActionIdx;
}

void UGOAPActionsComponent::AbortPlan()
{
	if (CurrentAction && CurrentAction->IsActionRunning())
	{
		CurrentAction->stop_action(AIOwner);
		if (AIOwner->IsInState(EAnimState::Anim))
		{
			ACharacter* Avatar = Cast<ACharacter>(AIOwner->GetPawn());
			if (Avatar)
				Avatar->StopAnimMontage();
		}
	}
	
	Reset();
}

void UGOAPActionsComponent::Reset()
{
	ActionIdx = 0;
	CurrentAction = nullptr;
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