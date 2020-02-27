#include "../Public/GOAPActionsComponent.h"
#include "../Public/GOAPController.h"
#include "../Public/GOAPAction.h"
#include "GameFramework/Character.h"

void UGOAPActionsComponent::OnRegister()
{
	Super::OnRegister();
	ActionIdx = 0;
	bPlanComplete = true;
	AIOwner = Cast<AGOAPController>(GetOwner());
}

void UGOAPActionsComponent::RunNextAction() 
{
	if (ActionIdx >= ActionsQueue.Num())
	{
		OnPlanCompleted.ExecuteIfBound();
		return;
	}
	CurrentAction = ActionsQueue[ActionIdx];
	UE_LOG(LogTemp, Warning, TEXT("Action index: %d"), ActionIdx);

	++ActionIdx;
	if (CurrentAction)
	{
		if (!CurrentAction->VerifyContext(AIOwner))
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Context"));
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
	ActionsQueue.Reset();
}

void UGOAPActionsComponent::QueueAction(UGOAPAction* Action)
{
	bPlanComplete = false;
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
	return bPlanComplete;
}