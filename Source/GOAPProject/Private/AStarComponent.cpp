// Fill out your copyright notice in the Description page of Project Settings.



#include "..\Public\AStarComponent.h"
#include "..\Public\GOAPController.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPGoal.h"
#include "..\Public\WorldState.h"

#include "Templates/Less.h"

UAStarComponent::UAStarComponent() :
	Super()
{
	MaxDepth = 10;
}
void UAStarComponent::OnRegister()
{
	Super::OnRegister();
	AIOwner = Cast<AGOAPController>(GetOwner());
}

void UAStarComponent::OnUnregister()
{
	AIOwner = nullptr;
	Super::OnUnregister();
}
bool UAStarComponent::Search(UGOAPGoal* goal, const UWorldState* controller_state, TArray<UGOAPAction*>& Plan) //graph needs to be V, E
{
	UE_LOG(LogTemp, Warning, TEXT("UAStarMachine::Search"));
	//Fringe is a priority queue in textbook A*
	//Use TArray's heap functionality to mimic a priority queue
	if (!goal || !controller_state)
		return false;
	TArray<TSharedPtr<FStateNode>> fringe;
	auto LessFn = TSharedPtrLess<FStateNode>(); //predicate, use in any heap functions
	fringe.Heapify(LessFn);

	//there are optimizations to be made here by someone smarter than me
	TSet<TSharedPtr<FStateNode>> closed_set; //probably have to figure out keyfuncs for this

	TSharedPtr<FStateNode> CurrentNode(new FStateNode());
	CurrentNode->SetupInitialNode(goal->container(), controller_state);

	//UE_LOG(LogTemp, Warning, TEXT("UAStarMachine::Search:\t set up initial node and push to fringe"));

	fringe.HeapPush(CurrentNode, LessFn);

	while (fringe.Num() != 0) {
		
		//UE_LOG(LogTemp, Warning, TEXT("UAStarMachine::Search:\t Fringe loop"));
		
		//pop the lowest cost node from p_queue
		fringe.HeapPop(CurrentNode, LessFn);
		if (!CurrentNode)
			break;
		
		//This is a regressive search
		//a goal node g is any node s.t. all values of the node's state match that of the initial state
		if (CurrentNode->IsGoal())
			break;
		//UE_LOG(LogTemp, Warning, TEXT("Check closed set"));

		if (CurrentNode->depth > MaxDepth)
		{
			CurrentNode = nullptr;
			continue;
		}
		//check closed set for current node
		//this prob doesn't work as expected rn.
		auto check_closed = closed_set.Find(CurrentNode);
		if (check_closed) 
		{
			//remove node from closed set if cost is better
			//o.w. just continue to next node
			if ((*check_closed)->cost() < CurrentNode->cost())
				closed_set.Remove(CurrentNode);
			else
				continue;
		}

		//Generate candidate edges (actions)
		TArray<UGOAPAction*> valid_actions;
		CurrentNode->FindActions(ActionTable, valid_actions);

		TSet<UGOAPAction*> visited_actions;
		//UE_LOG(LogTemp, Warning, TEXT("Add children to fringe"));
		for (auto action : valid_actions) 
		{
			
			check(IsValid(action)); //sanity check
			//verify context preconditions
			//skip action if it has already been visited for this node
			//UE_LOG(LogTemp, Warning, TEXT("          Checking action \"%s\""), *action->GetName());
			if (!action->VerifyContext(AIOwner) || visited_actions.Contains(action))
				continue;
			//UE_LOG(LogTemp, Warning, TEXT("          Mark visited"));
			//mark edge as visited for current node
			visited_actions.Add(action);
			
			//UE_LOG(LogTemp, Warning, TEXT("          create child node"));

			TSharedPtr<FStateNode> ChildNode(new FStateNode(CurrentNode, action));
			ChildNode->TakeAction(action);
			fringe.HeapPush(ChildNode, LessFn);
		}
	}
	
	if (CurrentNode)
	{
		generate_plan(CurrentNode, Plan);
		return true;
	}
	//UE_LOG(LogTemp, Warning, TEXT("No goal found!"));
	return false;
}

void UAStarComponent::ClearLookupTable()
{
	ActionTable.Empty();
}
void UAStarComponent::CreateLookupTable(TArray<UGOAPAction*>& Actions)
{
	//map effects symbols as keys to action objects for a regressive search
	for (auto Action : Actions) 
	{
		for (auto Effect : Action->effects) 
		{
			ActionTable.AddUnique(Effect.key, Action);
		}
	}
}

void UAStarComponent::generate_plan(TSharedPtr<FStateNode> FoundGoal, TArray<UGOAPAction*>& plan) 
{
	TSharedPtr<FStateNode> current = FoundGoal;
	//UE_LOG(LogTemp, Warning, TEXT("UAStarMachine::generate_plan"));
	//The starting node should have a nullptr for the parent node+edge
	while (current->previous().IsValid() && current->edge()) {
		FString action_name = current->edge()->GetName();
		//ScreenLog(FString::Printf(TEXT("adding action: %s"), *action_name));
		
		//edge is the action used to reach this node
		//the search is regressive so we add the actions in reverse order
		plan.Emplace(current->edge());
		//traverse to the parent node
		current = current->previous();
	}
}