#include "../Public/PlannerComponent.h"
#include "../Public/PlannerAsset.h"
#include "../Public/GOAPAction.h"
#include "../Public/GOAPGoal.h"
#include "../Public/PlannerService.h"



void UPlannerComponent::StartPlanner(UPlannerAsset& PlannerAsset)
{
	if (AIOwner == nullptr)
	{
		return;
	}
	ActionSet.Reserve(PlannerAsset.Actions.Num());
	for (auto* Action : PlannerAsset.Actions)
	{
		UGOAPAction* Copy = DuplicateObject<UGOAPAction>(Action, this);
		Copy->SetOwner(AIOwner, this);

		ActionSet.Emplace(Copy);
		
	}
	for (auto* Goal : PlannerAsset.Goals)
	{
		UGOAPGoal* Copy = DuplicateObject<UGOAPGoal>(Goal, this);
		Copy->SetOwner(*AIOwner, *this);
		Copy->OnWSUpdated(WorldState);
		Goals.Emplace(Copy);
	}
	for (auto& ServiceClass : PlannerAsset.Services)
	{
		Services.Add(NewObject<UPlannerService>(this, ServiceClass));
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

bool UPlannerComponent::IsRunningPlan() const
{
	return bPlanInProgress;
}

void UPlannerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 Index = 0; Index != Services.Num(); ++Index)
	{
		Services[Index]->TickService(*this, DeltaTime);
	}

	if (bWorldStateUpdated)
	{
		bWorldStateUpdated = false;

		//Should change this to a MC delegate
		for (auto* Goal : Goals)
		{
			Goal->OnWSUpdated(WorldState);
		}
	}

	if (bPlanUpdateNeeded)
	{
		UpdatePlanExecution();
	}

	//Do any replans last
	if (bReplanNeeded || !IsRunningPlan())
	{
		ProcessReplanRequest();
	}
}

void UPlannerComponent::SetWSProp(const EWorldKey& Key, const uint8& Value)
{
	uint8 Prev = WorldState.GetProp(Key);
	WorldState.SetProp(Key, Value);
	if (Prev != Value)
	{
		ScheduleWSUpdate();

		//Check for expected effects from current action, if any
		if (IsRunningPlan() && PlanBuffer[PlanHead] != nullptr)
		{
			for (auto& Effect : PlanBuffer[PlanHead]->GetEffects())
			{
				if (Effect.Key == Key )
				{
					if (PredictedWS.GetProp(Effect.Key) != Value)
					{
						ScheduleReplan();
					}
					return;
				}
			}
		}
		//Unhandled WS change causes replan
		ScheduleReplan();
	}
}

void UPlannerComponent::ScheduleWSUpdate()
{
	bWorldStateUpdated = true;
}

void UPlannerComponent::RequestExecutionUpdate()
{
	bPlanUpdateNeeded = true;
}

void UPlannerComponent::UpdatePlanExecution()
{
	bPlanUpdateNeeded = false;

	if (!PlanReachedEnd())
	{
		UGOAPAction* NextAction = PlanBuffer[PlanHead];
		if (NextAction != nullptr)
		{
			PredictedWS = WorldState;
			if (!NextAction->ValidatePlannerPreconditions(WorldState))
			{
				ClearCurrentPlan();
				ScheduleReplan();
				return;
			}
			for (auto& Effect : NextAction->GetEffects())
			{
				uint8 NextVal = Effect.Forward(WorldState.GetProp(Effect.Key));
				PredictedWS.SetProp(Effect.Key, NextVal);
			}
			NextAction->StartAction();
		}
	}
	else
	{
		bPlanInProgress = false;
		ScheduleReplan();
	}
}

void UPlannerComponent::OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result)
{
	if (Result == EPlannerTaskFinishedResult::Success)
	{
		//Apply values from the cached predicted WS
		for (auto& Effect : Action->GetEffects())
		{
			WorldState.SetProp(Effect.Key, PredictedWS.GetProp(Effect.Key));
		}
		//Still have to notify goals about new WS, but don't cause a replan
		for (auto& Goal : Goals)
		{
			Goal->OnWSUpdated(WorldState);
		}
		//Update the pointer and flag for the next tick
		PlanAdvance();
		RequestExecutionUpdate();
	}
	else
	{
		ClearCurrentPlan();
		ScheduleReplan();
	}
}

void UPlannerComponent::ScheduleReplan()
{
	bReplanNeeded = true;
}

void UPlannerComponent::ProcessReplanRequest()
{
	bReplanNeeded = false;
	auto InsistencePred = [](const UGOAPGoal& lhs, const UGOAPGoal& rhs) { 
		return lhs.GetInsistence() < rhs.GetInsistence(); 
	};
	TArray<UGOAPGoal*> ActiveGoals;
	ActiveGoals.Heapify(InsistencePred);
	for (auto* Goal : Goals)
	{
		if (Goal->IsValid() && (Goal->GetInsistence() > 0.f))
		{
			ActiveGoals.HeapPush(Goal, InsistencePred);
		}
	}

	//TODO: Pop active goals till found a valid plan
	if(ActiveGoals.Num() != 0)
	{
		UGOAPGoal* Top;
		ActiveGoals.HeapPop(Top, InsistencePred);
		//Search here
		if (IsRunningPlan() && PlanBuffer[PlanHead] != nullptr)
		{
			PlanBuffer[PlanHead]->AbortAction();
		}
		StartNewPlan(ActionSet); //for now
	}
	else
	{
		if (IsRunningPlan() && PlanBuffer[PlanHead] != nullptr)
		{
			PlanBuffer[PlanHead]->AbortAction();
			ClearCurrentPlan();
		}
	}
}

void UPlannerComponent::SetWSPropInternal(const EWorldKey& Key, const uint8& Value)
{
	WorldState.SetProp(Key, Value);
}


//Ring buffer stuff
bool UPlannerComponent::PlanReachedEnd()
{
	return PlanHead == PlanTail;
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
	bPlanInProgress = true;
	//pretty sure we want to do this on the same frame
	UpdatePlanExecution();
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
	while (PlanHead != PlanTail)
	{
		PlanBuffer[PlanHead] = nullptr;
		PlanHead = (PlanHead + 1) % BufferSize;
	}
	bPlanInProgress = false;
}

FString UPlannerComponent::GetDebugInfoString() const
{
	FString DebugInfo;
	DebugInfo += FString::Printf(TEXT("PlannerAsset: %s %d hmm\n"), *GetNameSafe(Asset), ActionSet.Num());

	DebugInfo += FString(TEXT("World State:\n"));
	UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWorldKey"), true);
	if (Enum != nullptr)
	{
		for (uint32 idx = 0; idx < WorldState.Num(); ++idx)
		{

			FString KeyName = Enum->GetNameStringByValue(idx);
			DebugInfo += FString::Printf(TEXT("    %s: %d\n"), *KeyName, WorldState.GetProp((EWorldKey)idx));
		}
	}
	for (auto* Goal : Goals)
	{
		if (!Goal)
			continue;
		FString GoalName = Goal ? Goal->GetTaskName() : FString(TEXT("None"));
		FString Valid = Goal->IsValid() ? FString(TEXT("Is")) : FString(TEXT("Is not"));
		DebugInfo += FString::Printf(TEXT("Goal: %s | %s valid\n"), *GoalName, *Valid);
	}
	for (auto* Action : ActionSet)
	{
		FString ActionName = Action ? Action->GetActionName() : FString(TEXT("None"));
		FString OpName = Action && Action->GetOperator() ? Action->GetOperator()->GetName() : FString(TEXT("None"));
		DebugInfo += FString::Printf(TEXT("Action: %s\n"), *ActionName);
		DebugInfo += FString::Printf(TEXT("    Pre: %d | Eff: %d\n"), Action->GetPreconditions().Num(), Action->GetEffects().Num());
		DebugInfo += FString::Printf(TEXT("    Op: %s\n"), *OpName);
	}
	for (uint32 Idx = PlanHead; Idx != PlanTail; Idx = (Idx + 1) % BufferSize)
	{
		UGOAPAction* Action = PlanBuffer[Idx];
		FString ActionName = Action ? Action->GetActionName() : FString(TEXT("None"));
		DebugInfo += FString::Printf(TEXT("Plan Step: %s\n"), *ActionName);
	}
	return DebugInfo;
}