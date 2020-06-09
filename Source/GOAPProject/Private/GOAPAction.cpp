

#include "..\Public\GOAPAction.h"

#include "..\Public\PlannerBrainComponent.h"

#include "Actions/PawnAction.h"
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

EActionStatus UGOAPAction::StartAction() 
{
	bIsRunning = true;
	
	UPawnAction* OperatorCopy = Operator ? DuplicateObject<UPawnAction>(Operator, GetOuter()) : nullptr;

	if (!AIOwner || !Operator)
	{
		return EActionStatus::kFailed;
	}

	Operator->SetActionObserver(FPawnActionEventDelegate::CreateUObject(this, &UGOAPAction::OnActionEvent));
	const bool bResult = AIOwner->PerformAction(*Operator, EAIRequestPriority::Logic, this);
	if (bResult)
	{
		return EActionStatus::kRunning;
	}
	return EActionStatus::kFailed;
}

void UGOAPAction::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	const bool bSuccess = (Result == EPlannerTaskFinishedResult::Success);
	UBrainComponent* BrainComp = AIOwner->BrainComponent;
	FAIMessage::Send(BrainComp, FAIMessage(UGOAPAction::ActionFinished, this, bSuccess));
}


void UGOAPAction::OnActionEvent(UPawnAction& Action, EPawnActionEventType::Type Event)
{
}

void UGOAPAction::AbortAction()
{
}

bool UGOAPAction::IsActionRunning()
{
	return bIsRunning;
}