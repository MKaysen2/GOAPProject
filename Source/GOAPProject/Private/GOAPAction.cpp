

#include "..\Public\GOAPAction.h"

#include "..\Public\AITask_AnimMontage.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "Tasks/AITask.h"
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

UAITask* UGOAPAction::GetOperator()
{
	return nullptr;
}

EActionResult UGOAPAction::StartAction() 
{
	EActionResult Result = EActionResult::Failed;
	Operator = GetOperator();
	if (AIOwner && Operator)
	{
		Operator->ReadyForActivation();
	}
	return Result;
}

void UGOAPAction::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	const bool bSuccess = (Result == EPlannerTaskFinishedResult::Success);
	UBrainComponent* BrainComp = AIOwner->BrainComponent;
	FAIMessage::Send(BrainComp, FAIMessage(UGOAPAction::ActionFinished, this, bSuccess));
}

void UGOAPAction::OnOperatorEnded()
{
	FinishAction(EPlannerTaskFinishedResult::Success);
}

EActionResult UGOAPAction::AbortAction()
{
	//Clear observers
	return EActionResult::Aborted;
}

UAITask* UAIAct_Animate::GetOperator()
{
	if (!Montage)
	{
		return nullptr;
	}
	UAITask_AnimMontage* MontageTask = UAITask_AnimMontage::AIAnimMontage(AIOwner, Montage);
	if (!MontageTask)
	{
		return nullptr;
	}
	MontageTask->OnMontageTaskEnded.AddUObject(this, &UGOAPAction::OnOperatorEnded);
	return MontageTask;
}