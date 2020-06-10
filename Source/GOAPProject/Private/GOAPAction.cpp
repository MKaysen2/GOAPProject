

#include "..\Public\GOAPAction.h"

#include "..\Public\PlannerBrainComponent.h"
#include "Actions/PawnActionsComponent.h"
#include "Actions/PawnAction.h"
#include "Actions/PawnAction_Wait.h"
#include "AIController.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogAction);

const FName UGOAPAction::MontageCompleted = TEXT("MontageCompleted");
const FName UGOAPAction::MontageBlendingOut = TEXT("MontageBlendingOut");
const FName UGOAPAction::ActionFinished = TEXT("ActionFinished");

UGOAPAction::UGOAPAction() : Super()
{
	
}

UGOAPAction::UGOAPAction(const int& Cost) :
	Super(),
	Preconditions(),
	Effects(),
	EdgeCost(Cost),
	Operator(nullptr)
{
}

void UGOAPAction::AddEffect(const EWorldKey& Key, const FAISymEffect& Effect)
{
	Effects.Add(Effect);
}

void UGOAPAction::AddPrecondition(const EWorldKey& Key, const uint8& Value)
{
	Preconditions.Add({ Key, Value });
}

void UGOAPAction::InitPreconditions()
{
}

void UGOAPAction::InitEffects()
{
}

void UGOAPAction::InitAction(AAIController* Controller)
{
	AIOwner = Controller;
	InitPreconditions();
	InitEffects();
}

EActionResult UGOAPAction::StartAction() 
{
	bIsRunning = true;
	
	UPawnAction* OperatorCopy = Operator ? DuplicateObject<UPawnAction>(Operator, GetOuter()) : nullptr;
	EActionResult Result = EActionResult::Failed;

	if (AIOwner && Operator)
	{
		UPawnAction_Wait* WaitCopy = Cast<UPawnAction_Wait>(OperatorCopy);
		if (WaitCopy)
		{
			WaitCopy->TimeToWait = TimeToWait;
		}
		OperatorCopy->SetActionObserver(FPawnActionEventDelegate::CreateUObject(this, &UGOAPAction::OnActionEvent));
		const bool bResult = AIOwner->PerformAction(*Operator, EAIRequestPriority::Logic, this);
		if (bResult)
		{
			Result = EActionResult::Running;
		}
	}
	return Result;
}

void UGOAPAction::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	const bool bSuccess = (Result == EPlannerTaskFinishedResult::Success);
	UBrainComponent* BrainComp = AIOwner->BrainComponent;
	FAIMessage::Send(BrainComp, FAIMessage(UGOAPAction::ActionFinished, this, bSuccess));
}


void UGOAPAction::OnActionEvent(UPawnAction& Action, EPawnActionEventType::Type Event)
{

	
	if (TaskStatus == EActionStatus::Active)
	{
		if (Event == EPawnActionEventType::FinishedExecution || Event == EPawnActionEventType::FailedToStart)
		{
			const bool bSucceeded = (Action.GetResult() == EPawnActionResult::Success);
			FinishAction((bSucceeded ? EPlannerTaskFinishedResult::Success : EPlannerTaskFinishedResult::Failure));
		}
		else if (Event == EPawnActionEventType::FinishedAborting)
		{
			FinishAction(EPlannerTaskFinishedResult::Failure);

		}
	}
	else if (TaskStatus == EActionStatus::Aborting)
	{
		if (Event == EPawnActionEventType::FinishedAborting ||
			Event == EPawnActionEventType::FinishedExecution || Event == EPawnActionEventType::FailedToStart)
		{
			FinishAction(EPlannerTaskFinishedResult::Failure);
		}
	}
}

EActionResult UGOAPAction::AbortAction()
{
	return (AIOwner != nullptr
		&& AIOwner->GetActionsComp() != nullptr
		&& AIOwner->GetActionsComp()->AbortActionsInstigatedBy(this, EAIRequestPriority::Logic) > 0)
		? EActionResult::Running : EActionResult::Aborted;
}

bool UGOAPAction::IsActionRunning()
{
	return bIsRunning;
}