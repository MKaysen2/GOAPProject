#include "..\Public\StateNode.h"
#include "..\Public\WorldState.h"
#include "..\Public\GOAPAction.h"

FStateNode::FStateNode( const FWorldState& InitialState, const TArray<FWorldProperty>& SymbolSet ) :
	CurrentState(MakeShared<FWorldState>(InitialState)),
	GoalState(MakeShared<FWorldState>(InitialState)), //two copies of the initial world state
	ParentNode(nullptr),
	ParentEdge(nullptr),
	UnsatisfiedKeys(),
	PropFlags(false, (uint8)EWorldKey::SYMBOL_MAX),
	ForwardCost(0),
	Heuristic(0),
	TotalCost(0),
	Closed(false),
	Depth(0)
{
	for (const auto& Symbol : SymbolSet)
	{
		
	}
}

FStateNode::FStateNode(const FStateNode& Node) :
	CurrentState(MakeShared<FWorldState>(Node.CurrentState.Get())),
	GoalState(Node.GoalState),
	ParentNode(Node.AsShared()),
	ParentEdge(nullptr),
	UnsatisfiedKeys(Node.UnsatisfiedKeys),
	PropFlags(Node.PropFlags),
	ForwardCost(Node.ForwardCost),
	Heuristic(Node.Heuristic),
	TotalCost(Node.TotalCost),
	Closed(false),
	Depth(Node.Depth + 1)
{

}

int FStateNode::GetCost() const 
{
	return TotalCost;
}

int FStateNode::GetDepth() const
{
	return Depth;
}

void FStateNode::MarkClosed()
{
	Closed = true;
}

void FStateNode::MarkOpened()
{
	Closed = false;
}

bool FStateNode::IsClosed() const
{
	return Closed;
}

void FStateNode::SetUnsatisfied(EWorldKey Key)
{

}

int FStateNode::GetForwardCost() const
{
	return ForwardCost;
}

bool FStateNode::IsGoal() 
{
	return Heuristic <= 0;
}

void FStateNode::GetNeighboringEdges(const LookupTable& ActionMap, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions)
{
	//change this
	for (const auto& Key : UnsatisfiedKeys)
	{
		ActionMap.MultiFind(Key, out_actions);
	}
}

void FStateNode::LogNode() const
{
	if (ParentEdge.IsValid())
	{
		UE_LOG(LogWS, Warning, TEXT("Parent edge: %.8s"), *ParentEdge->GetName());
	}
	else
	{
		UE_LOG(LogWS, Warning, TEXT("Node (start)"));
	}
	CurrentState->LogWS();
}


void FStateNode::ReParent(const FStateNode& OtherNode)
{
	ParentNode = OtherNode.ParentNode;
	ParentEdge = OtherNode.ParentEdge;
	//this should be correct since we're swapping the parent
	ForwardCost = OtherNode.ForwardCost;
	//Also need to change the depth
	Depth = OtherNode.Depth;
	//Unsatisfied should not be different so we won't change that
	CacheTotalCost();
}

bool FStateNode::ChainBackward(UGOAPAction& Action)
{

	ParentEdge = &Action;
	//resolve worldstate
	for (const auto& Effect : Action.GetEffects())
	{
		InvertEffect(Effect.Key);
	}

	for (const auto& Precondition : Action.GetPreconditions())
	{
		AddPrecondition(Precondition.Key, Precondition.Value);
	}
	CurrentState->CacheArrayTypeHash();

	//add cost of action to produce current total forward cost
	ForwardCost += Action.Cost();
	CacheTotalCost();
	return true;
}

bool FStateNode::InvertEffect(const EWorldKey& Key)
{
	uint8 DistanceBefore = GoalState->HeuristicDist(Key, CurrentState->GetProp(Key));
	uint8 NewValue = GoalState->GetProp(Key); //Replace with inverse effect call
	CurrentState->SetProp(Key, NewValue);
	uint8 DistanceAfter = GoalState->HeuristicDist(Key, NewValue);
	Heuristic += (DistanceAfter - DistanceBefore);
	return (DistanceAfter < DistanceBefore);
}

void FStateNode::AddPrecondition(const EWorldKey& Key, const uint8& Value)
{
	CurrentState->SetProp(Key, Value);
	uint8 Distance = GoalState->HeuristicDist(Key, Value);
	if (Distance != 0)
	{
		UnsatisfiedKeys.Add(Key);
		Heuristic += Distance;
	}
}

void FStateNode::CacheTotalCost()
{
	TotalCost = ForwardCost + Heuristic;
}

bool FStateNode::IsSatisfied(EWorldKey Key) const
{
	uint8 Idx = (uint8)Key;
	return PropFlags[Idx];
}