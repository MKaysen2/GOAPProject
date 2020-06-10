

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
}

bool UGOAPAction::SetOperatorParams()
{
	return false;
}

EActionResult UGOAPAction::StartAction() 
{
	EActionResult Result = EActionResult::Failed;
	return Result;
}

void UGOAPAction::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	const bool bSuccess = (Result == EPlannerTaskFinishedResult::Success);
	UBrainComponent* BrainComp = AIOwner->BrainComponent;
	FAIMessage::Send(BrainComp, FAIMessage(UGOAPAction::ActionFinished, this, bSuccess));
}

EActionResult UGOAPAction::AbortAction()
{
	return EActionResult::Aborted;
}
