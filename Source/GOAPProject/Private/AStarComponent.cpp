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

	while (fringe.Num() != 0) 
	{

		//pop the lowest cost node from p-queue
		fringe.HeapPop(CurrentNode, LessFn);
		if (!CurrentNode)
			break;
		
		//This is a regressive search
		//a goal node g is any node s.t. all values of the node's state match that of the initial state
		if (CurrentNode->IsGoal())
			break;

		if (CurrentNode->Depth > MaxDepth)
		{
			//Do not want to accidentally generate partial plan
			//if last node in fringe went over MaxDepth
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
		TArray<UGOAPAction*> CandidateActions;
		CurrentNode->FindActions(ActionTable, CandidateActions);

		TSet<UGOAPAction*> VisitedActions;
		for (auto Action : CandidateActions) 
		{
			
			check(IsValid(Action)); //sanity check

			//verify context preconditions
			//skip action if it has already been visited for this node
			if (!Action->VerifyContext(AIOwner) || VisitedActions.Contains(Action))
				continue;
			
			//mark edge as visited for current node
			VisitedActions.Add(Action);

			//Create the Child node 
			TSharedPtr<FStateNode> ChildNode(new FStateNode(CurrentNode, Action));
			
			//Apply the action and add node to fringe
			ChildNode->TakeAction(Action);
			fringe.HeapPush(ChildNode, LessFn);
		}
	}
	
	if (CurrentNode)
	{
		generate_plan(CurrentNode, Plan);
		return true;
	}
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

	//The starting node should have a nullptr for the parent node+edge
	while (current->previous().IsValid() && current->edge()) {
		FString action_name = current->edge()->GetName();
		
		//edge is the action used to reach this node
		//the search is regressive so we add the actions in reverse order
		plan.Emplace(current->edge());
		//traverse to the parent node
		current = current->previous();
	}
}