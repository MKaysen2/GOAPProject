

#include "..\Public\GOAPAction.h"

#include "..\Public\AITask_AnimMontage.h"
#include "..\Public\AITask_Operator.h"
#include "BrainComponent.h"
#include "..\Public\PlannerComponent.h"
#include "AIController.h"
#include "Tasks/AITask.h"
#include "GameFramework/Character.h"
#include "BlueprintNodeHelpers.h"

DEFINE_LOG_CATEGORY(LogAction);

const FName UGOAPAction::MontageCompleted = TEXT("MontageCompleted");
const FName UGOAPAction::MontageBlendingOut = TEXT("MontageBlendingOut");
const FName UGOAPAction::ActionFinished = TEXT("ActionFinished");

UGOAPAction::UGOAPAction() : 
	Super(),
	Preconditions(),
	Effects(),
	EdgeCost(1.0)
{
	
}

UGOAPAction::UGOAPAction(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
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

bool UGOAPAction::ValidatePlannerPreconditions(const FWorldState& WorldState)
{
	for (auto& Precondition : Preconditions)
	{
		if (!WorldState.CheckCondition(Precondition))
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

EActionResult UGOAPAction::StartAction()
{
	return EActionResult::Success;
}

void UGOAPAction::TickAction(float DeltaTime)
{
	//empty in base class
}

void UGOAPAction::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	if (OwnerComp)
	{
		OwnerComp->OnTaskFinished(this, Result);
	}
}

EActionResult UGOAPAction::AbortAction()
{
	//Clear observers
	return EActionResult::Aborted;
}

///// GOAPAction BlueprintBase//////////////

UGOAPAction_BlueprintBase::UGOAPAction_BlueprintBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UClass* StopAtClass = UGOAPAction_BlueprintBase::StaticClass();
	bExecuteHasImpl = BlueprintNodeHelpers::HasBlueprintFunction(FName("ReceiveExecuteAI"), *this, *StopAtClass);
	bTickHasImpl = BlueprintNodeHelpers::HasBlueprintFunction(FName("ReceiveTickAI"), *this, *StopAtClass);
	bAbortHasImpl = BlueprintNodeHelpers::HasBlueprintFunction(FName("ReceiveAbortAI"), *this, *StopAtClass);

}

EActionResult UGOAPAction_BlueprintBase::StartAction()
{
	if (AIOwner && bExecuteHasImpl)
	{
		ReceiveExecuteAI(AIOwner, AIOwner->GetPawn());
		return EActionResult::Running;
	}
	return EActionResult::Failed;
}

EActionResult UGOAPAction_BlueprintBase::AbortAction()
{
	BlueprintNodeHelpers::AbortLatentActions(*OwnerComp, *this);
	if (bAbortHasImpl)
	{
		if (AIOwner)
		{
			ReceiveAbortAI(AIOwner, AIOwner->GetPawn());
			return EActionResult::Running;
		}
	}
	return EActionResult::Aborted;
}

void UGOAPAction_BlueprintBase::FinishExecute(bool bSuccess)
{
	EPlannerTaskFinishedResult::Type Result(bSuccess ? EPlannerTaskFinishedResult::Success : EPlannerTaskFinishedResult::Failure);
	
	if (OwnerComp && !bIsAborting)
	{
		OwnerComp->OnTaskFinished(this, Result);
	}

}

void UGOAPAction_BlueprintBase::FinishAbort()
{
	if (OwnerComp && bIsAborting)
	{
		OwnerComp->OnTaskFinished(this, EPlannerTaskFinishedResult::Aborted);
	}
}

void UGOAPAction_BlueprintBase::TickAction(float DeltaTime)
{

}
/////////      UGOAPAction_Operator //////////////

UGOAPAction_Operator::UGOAPAction_Operator() : Super(), Operator(nullptr)
{

}

UGameplayTasksComponent* UGOAPAction_Operator::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	const UAITask* AITask = Cast<const UAITask>(&Task);
	return (AITask && AITask->GetAIController()) ? AITask->GetAIController()->GetGameplayTasksComponent() : Task.GetGameplayTasksComponent();
}

AActor* UGOAPAction_Operator::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	const UAITask* AITask = Cast<UAITask>(Task);
	if (AITask)
	{
		return AITask->GetAIController() ? AITask->GetAIController() : nullptr;
	}
	return nullptr;
}

AActor* UGOAPAction_Operator::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	const UAITask* AITask = Cast<UAITask>(Task);
	if (AITask)
	{
		return AITask->GetAIController() ? AITask->GetAIController()->GetPawn() : nullptr;
	}
	return nullptr;
}

void UGOAPAction_Operator::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UAITask_Operator* OpTask = Cast<UAITask_Operator>(&Task);
	if (OpTask && OpTask->GetState() == EGameplayTaskState::Finished && OpTask->GetOpStatus() == EOpStatus::Succeeded)
	{
		FinishAction(EPlannerTaskFinishedResult::Success);
	}
	else
	{
		FinishAction(EPlannerTaskFinishedResult::Failure);
	}
}


UAITask* UGOAPAction_Operator::GetOperator()
{
	return Operator;
}

EActionResult UGOAPAction_Operator::StartAction()
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

void UGOAPAction_Operator::FinishAction(EPlannerTaskFinishedResult::Type Result)
{
	const bool bSuccess = (Result == EPlannerTaskFinishedResult::Success);
	OpInstance = nullptr;

	if (OwnerComp)
	{
		OwnerComp->OnTaskFinished(this, Result);
	}
}

void UGOAPAction_Operator::OnOperatorEnded()
{
	FinishAction(EPlannerTaskFinishedResult::Success);
}

EActionResult UGOAPAction_Operator::AbortAction()
{
	//Clear observers
	if (OpInstance)
	{
		OpInstance->ExternalCancel();
		OpInstance = nullptr;
	}
	return EActionResult::Aborted;
}
