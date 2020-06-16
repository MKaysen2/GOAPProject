#include "../Public/PlannerComponent.h"
#include "../Public/PlannerAsset.h"
#include "../Public/GOAPAction.h"

void UPlannerComponent::OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result)
{

}

void UPlannerComponent::StartPlanner(UPlannerAsset& PlannerAsset)
{
	ActionSet = PlannerAsset.Actions;
	for (auto* Action : ActionSet)
	{
		Action->InitAction(AIOwner);
	}
	Asset = &PlannerAsset;
	BufferSize = PlannerAsset.MaxPlanSize + 1;
	PlanBuffer.Init(nullptr, BufferSize);
}

void UPlannerComponent::RunAllActions()
{
	StartNewPlan(ActionSet);
	RequestExecutionUpdate();

}

FString UPlannerComponent::GetDebugInfoString() const
{
	FString DebugInfo;
	DebugInfo += FString::Printf(TEXT("PlannerAsset: %s %d hmm\n"), *GetNameSafe(Asset), ActionSet.Num());

	for (auto* Action : ActionSet)
	{
		FString ActionName = Action ? Action->GetActionName() : FString(TEXT("None"));
		DebugInfo += FString::Printf(TEXT("Action: %s\n"), *ActionName);
	}
	for (uint32 Idx = PlanHead; Idx != PlanTail; Idx = (Idx + 1) % BufferSize)
	{
		UGOAPAction* Action = PlanBuffer[Idx];
		FString ActionName = Action ? Action->GetActionName() : FString(TEXT("None"));
		DebugInfo += FString::Printf(TEXT("Plan Step: %s\n"), *ActionName);
	}
	return DebugInfo;
}

void UPlannerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bPlanUpdateNeeded)
	{
		UpdatePlanExecution();
	}

}

void UPlannerComponent::RequestExecutionUpdate()
{
	bPlanUpdateNeeded = true;
}

void UPlannerComponent::UpdatePlanExecution()
{
	bPlanUpdateNeeded = false;

	bool bReachedEnd = PlanAdvance();
	if (!bReachedEnd)
	{
		UGOAPAction* NextAction = PlanBuffer[PlanHead];
		if (NextAction != nullptr)
		{
			NextAction->StartAction();
		}
	}
}

bool UPlannerComponent::PlanAdvance()
{
	PlanBuffer[PlanHead] = nullptr; //clear previous action
	//increment pointer
	PlanFull = false;

	PlanHead = (PlanHead + 1) % BufferSize;

	//return whether we've reached the end of the buffer
	return (PlanHead == PlanTail);
}

void UPlannerComponent::StartNewPlan(TArray<UGOAPAction*>& Plan)
{
	ClearCurrentPlan();
	for (auto* Action : Plan)
	{
		AddAction(Action);
	}
}

void UPlannerComponent::AddAction(UGOAPAction* Action)
{
	if (PlanFull)
	{
		return;
	}
	PlanBuffer[PlanTail] = Action;
	PlanTail = (PlanTail + 1) % BufferSize;
	if (PlanTail == PlanHead)
	{
		PlanFull = true;
	}
}

void UPlannerComponent::ClearCurrentPlan()
{
	PlanFull = false;
	for (uint32 idx = 0; idx < BufferSize; ++idx)
	{
		PlanBuffer[idx] = nullptr;
	}
	PlanTail = PlanHead;
}
