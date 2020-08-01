#include "../Public/PlannerComponent.h"
#include "../Public/PlannerAsset.h"
#include "../Public/GOAPAction.h"
#include "../Public/GOAPGoal.h"
#include "../Public/StateNode.h"
#include "../Public/PlannerService.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
//FAStarPlanner 
//Should move this into the same file as StateNode
bool FAStarPlanner::Search(const TArray<FWorldProperty>& GoalCondition, const FWorldState& InitialState, TArray<UGOAPAction*>& Plan)
{
	//Fringe is a priority queue in textbook A*
	//Use TArray's heap functionality to mimic a priority queue

	FPriorityQueue Fringe;

	//To save time, ALL nodes are added to a single set, and keep track of whether they're closed
	//This does mean that we're using additional space, but it's easier and faster, I think
	TSet<NodePtr, FStateNode::SetKeyFuncs> NodePool;

	NodePtr CurrentNode = MakeShared<FStateNode>(InitialState, GoalCondition);

	Fringe.Push(CurrentNode);
	NodePool.Add(CurrentNode);
	//TODO: empty the fringe before exiting
	while (Fringe.Num() != 0)
	{

		//pop the lowest cost node from p-queue
		Fringe.Pop(CurrentNode);
		if (!CurrentNode.IsValid())
		{
			break;
		}
		CurrentNode->MarkClosed();
		//This is a regressive search
		//a goal node g is any node s.t. all values of the node's state match that of the initial state
		if (CurrentNode->IsGoal())
		{
			break;
		}

		if (CurrentNode->GetDepth() > MaxDepth)
		{
			//Do not want to accidentally generate partial plan
			//if last node in fringe went over MaxDepth
			CurrentNode = nullptr;
			continue;
		}

		//Generate candidate edges (actions)
		TArray<TWeakObjectPtr<UGOAPAction>> CandidateEdges;
		CurrentNode->GetNeighboringEdges(EdgeTable, CandidateEdges);

		TSet<UGOAPAction*> VisitedActions;
		for (auto ActionHandle : CandidateEdges)
		{


			if (!ActionHandle.IsValid())
			{
				UE_LOG(LogAction, Error, TEXT("Bad Action access in planner!!"));
				UE_LOG(LogAction, Error, TEXT("You probably dumped the ActionSet somewhere, again"));
				continue;
			}

			//Can move this stuff into GenerateNeighbors
			UGOAPAction* Action = ActionHandle.Get();
			//verify context preconditions
			//skip action if it has already been visited for this node
			if (!Action->VerifyContext() || VisitedActions.Contains(Action))
			{
				continue;
			}

			//mark edge as visited for current node
			VisitedActions.Add(Action);

			//Create the Child node 
			NodePtr ChildNode = MakeShared<FStateNode>(*CurrentNode);
			if (!ChildNode.IsValid())
			{
				continue;
			}
			if (!ChildNode->ChainBackward(*Action))
			{
				continue;
			}
			//check if node exists already
			const NodePtr* FindNode = NodePool.Find(ChildNode);
			if (FindNode != nullptr && FindNode->IsValid())
			{
				NodePtr ExistingNode = *FindNode;
				if (ChildNode->GetForwardCost() < ExistingNode->GetForwardCost())
				{
					ExistingNode->ReParent(*ChildNode);
					if (ExistingNode->IsClosed())
					{
						ExistingNode->MarkOpened();
						Fringe.Push(ExistingNode);
					}
					else
					{
						Fringe.ReSort();
					}
				}
			}
			else
			{
				ChildNode->MarkOpened(); //just in case we haven't
				Fringe.Push(ChildNode);
				NodePool.Add(ChildNode);
			}
		}
		//If we run out of nodes, we don't want to still be referencing a valid node
		CurrentNode.Reset();
	}

	if (CurrentNode.IsValid())
	{
		const FStateNode* Node = CurrentNode.Get();
		while (Node && Node->ParentNode.IsValid())
		{
			Plan.Add(Node->ParentEdge.Get());
			Node = Node->ParentNode.Pin().Get();
		}
		return true;
	}
	return false;
}

void FAStarPlanner::AddAction(UGOAPAction* Action)
{
	for (const auto& Effect : Action->GetEffects())
	{
		EdgeTable.AddUnique(Effect.Key, Action);
	}
}

void FAStarPlanner::RemoveAction(UGOAPAction* Action)
{
	//Nothing for now
	for (const auto& Effect : Action->GetEffects())
	{
		EdgeTable.RemoveSingle(Effect.Key, Action);
	}
}

void FAStarPlanner::ClearEdgeTable()
{
	EdgeTable.Empty();
}

//UPlannerComponent
void UPlannerComponent::StartPlanner(UPlannerAsset& PlannerAsset)
{
	if (AIOwner == nullptr)
	{
		return;
	}
	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	if (BBComp && IsValid(PlannerAsset.BlackboardData))
	{
		BBComp->InitializeBlackboard(*PlannerAsset.BlackboardData);
		CacheBlackboardComponent(BBComp);
	}
	for (auto& KeyConfig : PlannerAsset.WSKeyDefaults)
	{
		if (KeyConfig.Type == EWSValueType::BBKey)
		{
			WorldState.SetProp(KeyConfig.KeyLHS, BBComp->GetKeyID(KeyConfig.BBKeyName));
		}
		else
		{
			WorldState.SetProp(KeyConfig.KeyLHS, KeyConfig.Value);
		}
	}
	ActionSet.Reserve(PlannerAsset.Actions.Num());
	for (auto* Action : PlannerAsset.Actions)
	{
		UGOAPAction* Copy = DuplicateObject<UGOAPAction>(Action, this);
		Copy->SetOwner(AIOwner, this);

		ActionSet.Emplace(Copy);
		AStarPlanner.AddAction(Copy);
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
	CurrentGoal = nullptr;
	Asset = &PlannerAsset;
	AStarPlanner.MaxDepth = PlannerAsset.MaxPlanSize;
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
	if (Prev != Value)
	{
		WorldState.SetProp(Key, Value);
		ScheduleWSUpdate();

		//Check for expected effects from current action, if any
		if (IsRunningPlan() && PlanBuffer[PlanHead] != nullptr)
		{
			for (auto& Effect : ExpectedEffects)
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

	UGOAPAction* NextAction = PlanBuffer[PlanHead];

	if (!PlanReachedEnd() && NextAction != nullptr)
	{
		ExpectedEffects.Reset();

		if (!NextAction->ValidatePlannerPreconditions(WorldState))
		{
			AbortPlan();
			ScheduleReplan();
			return;
		}

		{
			FWorldState PredictedState = FWorldState(WorldState); //need to save computed values in case later effects reference them
			for (auto& Effect : NextAction->GetEffects())
			{
				PredictedState.ApplyEffect(Effect);
				if (Effect.bExpected)
				{
					auto Idx = ExpectedEffects.Add(FAISymEffect());
					ExpectedEffects[Idx].Key = Effect.Key;
					ExpectedEffects[Idx].Value = PredictedState.GetProp(Effect.Key);
				}
			}
		}

		//TODO: check the return value for success/failure
		NextAction->StartAction();
		ActionStatus = EActionStatus::Active;
	}
	else
	{
		AbortPlan();
		ScheduleReplan();
	}
}

void UPlannerComponent::OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result)
{
	ActionStatus = EActionStatus::Inactive;
	if (Result == EPlannerTaskFinishedResult::Success)
	{
		//Apply values from task effects
		for (auto& Effect : Action->GetEffects())
		{
			//"Expected" effects from sensors shouldn't be applied
			//we also don't need to fail here since it'll be caught
			//by the preconditions of the next task
			if (!Effect.bExpected)
			{
				WorldState.ApplyEffect(Effect);
			}
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
	else if (Result == EPlannerTaskFinishedResult::Aborted)
	{
		//We finished a latent abort, so we don't want to apply effects
		//However, we might have started a new plan
		PlanAdvance();
		RequestExecutionUpdate();
	}
	else
	{
		AbortPlan();

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

	if (ActiveGoals.Num() == 0)
	{
		UE_LOG(LogAction, Warning, TEXT("No active goal"));
	}

	while(ActiveGoals.Num() != 0)
	{
		UGOAPGoal* Top;
		ActiveGoals.HeapPop(Top, InsistencePred);

		//Prefer not to interrupt the current plan if possible
		//May want to add a list of changes that would force a replan even if the current goal
		//is still the same
		if (Top == CurrentGoal)
		{
			return;
		}
		//Search here
		TArray<UGOAPAction*> NewPlan;

		bool bPlanFound = AStarPlanner.Search(Top->GetGoalCondition(), WorldState, NewPlan);
		//could not satisfy goal so go to next highest

		if (!bPlanFound)
		{
			continue;
		}
		//o.w
		if (IsRunningPlan() && PlanBuffer[PlanHead] != nullptr)
		{
			PlanBuffer[PlanHead]->AbortAction();
		}
		StartNewPlan(NewPlan); //for now
		return;
	}
	
	if (ActiveGoals.Num() != 0)
	{
		UE_LOG(LogAction, Warning, TEXT("Could not find plans for any active goals"));
	}
	if (IsRunningPlan() && PlanBuffer[PlanHead] != nullptr)
	{
		AbortPlan();
	}
}

void UPlannerComponent::SetWSPropInternal(const EWorldKey& Key, const uint8& Value)
{
	WorldState.SetProp(Key, Value);
}


//Ring buffer stuff
bool UPlannerComponent::PlanReachedEnd()
{
	return (PlanHead == PlanTail) && PlanBuffer[PlanHead] == nullptr;
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
	if (IsRunningPlan())
	{
		AbortPlan();
	}
	for (auto* Action : Plan)
	{
		AddAction(Action);
	}
	bPlanInProgress = true;
	//pretty sure we want to do this on the same frame
	UpdatePlanExecution();
}

void UPlannerComponent::AbortPlan()
{
	ExpectedEffects.Reset();
	if (PlanBuffer[PlanHead] != nullptr && ActionStatus == EActionStatus::Active)
	{
		EActionResult Result = PlanBuffer[PlanHead]->AbortAction();
		if (Result == EActionResult::Running)
		{
			ActionStatus = EActionStatus::Aborting;
			ClearAllButCurrentAction();
		}
	}
	else if (PlanBuffer[PlanHead] != nullptr && ActionStatus == EActionStatus::Aborting)
	{
		ClearAllButCurrentAction();
	}
	else
	{
		ClearAll();
	}
}

void UPlannerComponent::AddAction(UGOAPAction* Action)
{
	if (PlanTail == PlanHead && PlanBuffer[PlanHead] != nullptr)
	{
		TArray<UGOAPAction*> NewBuffer;
		NewBuffer.Init(nullptr, PlanBuffer.Num() * 2);
		int Idx = 0;
		do
		{
			NewBuffer[Idx] = PlanBuffer[PlanHead];
			PlanBuffer[PlanHead] = nullptr;
			PlanHead = (PlanHead + 1 % PlanBuffer.Num());
			++Idx;
		} while (PlanHead != PlanTail);

		PlanBuffer = NewBuffer;
		BufferSize = NewBuffer.Num();
	}
	PlanBuffer[PlanTail] = Action;
	PlanTail = (PlanTail + 1) % BufferSize;
}

void UPlannerComponent::ClearAllButCurrentAction()
{
	int32 StartIdx = (PlanHead + 1) % BufferSize;
	while (PlanHead != PlanTail)
	{
		PlanBuffer[StartIdx] = nullptr;
		StartIdx = (StartIdx + 1) % BufferSize;
	}
	PlanTail = (PlanHead + 1) % BufferSize;
	bPlanInProgress = false;

}

void UPlannerComponent::ClearAll()
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
		DebugInfo += FString::Printf(TEXT("Action: %s\n"), *ActionName);
		DebugInfo += FString::Printf(TEXT("    Pre: %d | Eff: %d\n"), Action->GetPreconditions().Num(), Action->GetEffects().Num());
	}
	for (uint32 Idx = PlanHead; Idx != PlanTail; Idx = (Idx + 1) % BufferSize)
	{
		UGOAPAction* Action = PlanBuffer[Idx];
		FString ActionName = Action ? Action->GetActionName() : FString(TEXT("None"));
		DebugInfo += FString::Printf(TEXT("Plan Step: %s\n"), *ActionName);
	}
	return DebugInfo;
}