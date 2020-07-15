#include "..\Public\StateNode.h"
#include "..\Public\WorldState.h"
#include "..\Public\GOAPAction.h"

FStateNode::FStateNode(const FWorldState& InitialState, const TArray<FWorldProperty>& SymbolSet) :
	CurrentState(MakeShared<FWorldState>(InitialState)),
	GoalState(MakeShared<FWorldState>(InitialState)), //two copies of the initial world state
	ParentNode(nullptr),
	ParentEdge(nullptr),
	UnsatisfiedKeys(),
	PropFlags(),
	ForwardCost(0),
	Heuristic(0),
	TotalCost(0),
	Closed(false),
	Depth(0),
	CachedHash(0)
{
	PropFlags.Init(false, CurrentState->Num());
	for (const auto& Symbol : SymbolSet)
	{
		AddPrecondition(Symbol);
	}
	CacheTypeHash(GetTypeHash(CurrentState.Get()));
	CacheTotalCost();
}

FStateNode::FStateNode(const FStateNode& Node) :
	CurrentState(MakeShared<FWorldState>(Node.CurrentState.Get())),
	GoalState(Node.GoalState),
	ParentNode(Node.AsShared()),
	ParentEdge(Node.ParentEdge),
	UnsatisfiedKeys(Node.UnsatisfiedKeys),
	PropFlags(Node.PropFlags),
	ForwardCost(Node.ForwardCost),
	Heuristic(Node.Heuristic),
	TotalCost(Node.TotalCost),
	Closed(Node.Closed),
	Depth(Node.Depth),
	CachedHash(Node.CachedHash)
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

int FStateNode::GetForwardCost() const
{
	return ForwardCost;
}

bool FStateNode::IsGoal() 
{
	return Heuristic <= 0;
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

void FStateNode::GetNeighboringEdges(const LookupTable& ActionMap, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions)
{
	//change this
	for (const auto& Key : UnsatisfiedKeys)
	{
		ActionMap.MultiFind(Key, out_actions);
	}
}

bool FStateNode::ChainBackward(UGOAPAction& Action)
{

	ParentEdge = &Action;
	Depth += 1;
	//resolve worldstate
	int32 CachedHeuristic = Heuristic;

	for (const auto& Effect : Action.GetEffects())
	{
		if (GetKeyRelevance(Effect.Key) == false)
		{
			continue;
		}
		//if the effect couldn't have happened, then fail
		//as the action couldn't have happened
		if (!InvertEffect(Effect.Key, Effect))
		{
			return false;
		}

	}

	for (const auto& Precondition : Action.GetPreconditions())
	{
		if (!AddPrecondition(Precondition))
		{
			return false;
		}
	}
	if (Heuristic > CachedHeuristic)
	{
		return false;
	}
	CacheTypeHash(GetTypeHash(CurrentState.Get()));

	//add cost of action to produce new forward cost
	ForwardCost += Action.Cost();
	//Also cache total cost for PQueue comparison
	CacheTotalCost();
	return true;
}

bool FStateNode::InvertEffect(const EWorldKey& Key, const FAISymEffect& Effect)
{
	uint8 CurVal = CurrentState->GetProp(Key);
	uint8 GroundVal = GoalState->GetProp(Key);
	uint8 PriorVal = Effect.Backward(CurVal, GroundVal);

	//The effect couldn't have occured if the forward call on the computed prior
	//produces a different value than what actually happened (the "current" value)
	if (Effect.Forward(PriorVal) != CurVal)
	{
		return false;
	}

	//This is actually fine
	CurrentState->SetProp(Key, PriorVal);
	if (PriorVal == GroundVal)
	{
		//if the effect produces a constant value, then the value COULD have been anything
		//Otherwise, there is an implicit precondition on the value since it needs to have held
		//for the found plan to be valid
		if (Effect.Op == ESymbolOp::Set)
		{
			SetKeyRelevance(Key, false);
		}
		UnsatisfiedKeys.Remove(Key);
	}

	Heuristic -= GoalState->HeuristicDist(Key, CurVal);
	Heuristic += GoalState->HeuristicDist(Key, PriorVal);

	return true;
}

bool FStateNode::AddPrecondition(const FWorldProperty& Precondition)
{
	EWorldKey Key = Precondition.Key;
	uint8 GroundVal = GoalState->GetProp(Key);
	uint8 CurVal = CurrentState->GetProp(Key);
	
	//if the precondition COULD NOT have been true, then we can skip it
	if (GetKeyRelevance(Key) == true)
	{
		return Precondition.Eval(CurVal);
	}
	uint8 NewVal = (Precondition.Eval(GroundVal) ? GroundVal : Precondition.MinSatisfyVal());
	CurrentState->SetProp(Key, NewVal);
	SetKeyRelevance(Key, true);
	int32 Distance = GoalState->HeuristicDist(Key, NewVal);
	if (Distance != 0)
	{
		UnsatisfiedKeys.Add(Key);
		Heuristic += Distance;
	}
	return true;
}

void FStateNode::CacheTotalCost()
{
	TotalCost = ForwardCost + Heuristic;
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