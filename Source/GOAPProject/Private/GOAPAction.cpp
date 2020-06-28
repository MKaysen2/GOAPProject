

#include "..\Public\GOAPAction.h"

#include "..\Public\AITask_AnimMontage.h"
#include "..\Public\AITask_Operator.h"
#include "BrainComponent.h"
#include "..\Public\PlannerComponent.h"
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

UGameplayTasksComponent* UGOAPAction::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	const UAITask* AITask = Cast<const UAITask>(&Task);
	return (AITask && AITask->GetAIController()) ? AITask->GetAIController()->GetGameplayTasksComponent() : Task.GetGameplayTasksComponent();
}

AActor* UGOAPAction::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	const UAITask* AITask = Cast<UAITask>(Task);
	if (AITask)
	{
		return AITask->GetAIController() ? AITask->GetAIController() : nullptr;
	}
	return nullptr;
}

AActor* UGOAPAction::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	const UAITask* AITask = Cast<UAITask>(Task);
	if (AITask)
	{
		return AITask->GetAIController() ? AITask->GetAIController()->GetPawn() : nullptr;
	}
	return nullptr;
}

void UGOAPAction::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	if (Task.GetState() == EGameplayTaskState::Finished)
	{
		FinishAction(EPlannerTaskFinishedResult::Success);
	}
}

void UGOAPAction::AddEffect(const EWorldKey& Key, const FAISymEffect& Effect)
{
	Effects.Add(Effect);
}

void UGOAPAction::AddPrecondition(const EWorldKey& Key, const uint8& Value)
{
	Preconditions.Add({ Key, Value });
}

bool UGOAPAction::ValidatePlannerPreconditions(const FWorldState& WorldState)
{
	for (auto& Precondition : Preconditions)
	{
		if (Precondition.Value != WorldState.GetProp(Precondition.Key))
		{
			return false;
		}
	}
	return true;
}

void UGOAPAction::InitAction(AAIController* Controller)
{
	AIOwner = Controller;
}

void UGOAPAction::SetOwner(AAIController* Controller, UPlannerComponent* OwnerComponent)
{
	AIOwner = Controller;
	OwnerComp = OwnerComponent;
}

UAITask* UGOAPAction::GetOperator()
{
	return Operator;
}

EActionResult UGOAPAction::StartAction() 
{
	EActionResult Result = EActionResult::Failed;
	UAITask_Operator* OpCopy = DuplicateObject<UAITask_Operator>(Operator, this);
	if (AIOwner && OpCopy)
	{
		OpCopy->InitAITask(*AIOwner, *this);
		OpCopy->ReadyForActivation();
		OpInstance = OpCopy;
		Result = (OpCopy->GetState() != EGameplayTaskState::Finished ? EActionResult::Running : EActionResult::Failed);
	}
	return Result;
}

void UGOAPAction::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	const bool bSuccess = (Result == EPlannerTaskFinishedResult::Success);
	OpInstance = nullptr;

	if (OwnerComp)
	{
		OwnerComp->OnTaskFinished(this, Result);
	}
}

void UGOAPAction::OnOperatorEnded()
{
	FinishAction(EPlannerTaskFinishedResult::Success);
}

EActionResult UGOAPAction::AbortAction()
{
	//Clear observers
	if (OpInstance)
	{
		OpInstance->ExternalCancel();
		OpInstance = nullptr;
	}
	return EActionResult::Aborted;
}
