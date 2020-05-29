// Fill out your copyright notice in the Description page of Project Settings.



#include "..\Public\AStarComponent.h"
#include "..\Public\GOAPController.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPGoal.h"
#include "..\Public\WorldState.h"
#include "Templates/IsTrivial.h"

#include "Templates/Less.h"

UAStarComponent::UAStarComponent() :
	Super()
{
	MaxDepth = 10;
	UE_LOG(LogTemp, Warning, TEXT("Is FWorldProperty Trivial: %d"), TIsTrivial<FWorldProperty>::Value);
	UE_LOG(LogTemp, Warning, TEXT("Is FVector Trivial?: %d"), TIsTrivial<FVector>::Value);
	UE_LOG(LogTemp, Warning, TEXT("Is TSharedRef<FWorldProperty> Trivial: %d"), TIsTrivial<TSharedRef<FWorldProperty>>::Value);
	UE_LOG(LogTemp, Warning, TEXT("Is FWorldState Trivial: %d"), TIsTrivial<FWorldState>::Value);
}

void UAStarComponent::OnRegister()
{
	Super::OnRegister();
	AIOwner = Cast<AAIController>(GetOuter());
}

void UAStarComponent::OnUnregister()
{
	AIOwner = nullptr;
	Super::OnUnregister();
}

TSharedPtr<FStateNode> UAStarComponent::Search(UGOAPGoal* Goal, const FWorldState& InitialState) //graph needs to be V, E
{
	//Fringe is a priority queue in textbook A*
	//Use TArray's heap functionality to mimic a priority queue
	if (!IsValid(Goal))
	{
		return nullptr;
	}
	FPriorityQueue Fringe;

	//To save time, ALL nodes are added to a single set, and keep track of whether they're closed
	//This does mean that we're using additional space, but it's easier and faster, I think
	TSet<NodePtr, FStateNode::SetKeyFuncs> NodePool;

	NodePtr CurrentNode = MakeShared<FStateNode>(InitialState, Goal->GetSymbolSet());

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
		CurrentNode->GetNeighboringEdges(ActionTable, CandidateEdges);

		TSet<UGOAPAction*> VisitedActions;
		for (auto ActionHandle : CandidateEdges) 
		{

			
			if (!ActionHandle.IsValid())
			{
				UE_LOG(LogAction, Error, TEXT("Bad Action access in planner!!"));
				UE_LOG(LogAction, Error, TEXT("You probably dumped the ActionSet somewhere, again"));
				continue;
			}
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
			const auto* FindNode = NodePool.Find(ChildNode);
			if(FindNode != nullptr && FindNode->IsValid())
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
	}
	
	if (CurrentNode.IsValid())
	{
		return CurrentNode;
	}
	return nullptr;
}

void UAStarComponent::AddAction(UGOAPAction* Action)
{
	for (const auto& Effect : Action->GetEffects())
	{
		ActionTable.AddUnique(Effect.Key, Action);
	}
}

void UAStarComponent::RemoveAction(UGOAPAction* Action)
{
	//Nothing for now
	for (const auto& Effect : Action->GetEffects())
	{
		ActionTable.RemoveSingle(Effect.Key, Action);
	}
}

void UAStarComponent::ClearLookupTable()
{
	ActionTable.Empty();
}
void UAStarComponent::CreateLookupTable(TArray<UGOAPAction*>& Actions)
{
	//map effects symbols as keys to action objects for a regressive search
	for (auto* Action : Actions) 
	{
		for (const auto& Effect : Action->GetEffects()) 
		{
			ActionTable.AddUnique(Effect.Key, Action);
		}
	}
}