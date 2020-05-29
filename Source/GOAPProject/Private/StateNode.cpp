#include "..\Public\StateNode.h"
#include "..\Public\WorldState.h"
#include "..\Public\GOAPAction.h"

FStateNode::FStateNode() :
	CurrentState(MakeShared<FWorldState>()),
	GoalState(nullptr),
	ParentNode(nullptr),
	ParentEdge(nullptr),
	PropFlags(false, (uint8)EWorldKey::SYMBOL_MAX),
	ForwardCost(0),
	NumUnsatisfied(0),
	Depth(0),
	Closed(false)
{
	//UE_LOG(LogTemp, Warning, TEXT("FStateNode default constructor"));
	//Not sure this is ever used
}

FStateNode::FStateNode(const TArray<FWorldProperty>& GoalSet, TSharedPtr<FWorldState> InitialState) :
	CurrentState(MakeShared<FWorldState>()),
	GoalState(InitialState),
	ParentNode(nullptr),
	ParentEdge(nullptr),
	PropFlags(false, (uint8)EWorldKey::SYMBOL_MAX),
	ForwardCost(0),
	NumUnsatisfied(0),
	Depth(0),
	Closed(false)
{
	//This should really be in a static method, it's extremely confusing the way it is now
	for (auto Property : GoalSet)
	{
		CurrentState->Add(Property);
		CurrentState->ValidateProperty(GoalState.Get(), Property.Key);
	}
	CurrentState->CacheArrayTypeHash();
	NumUnsatisfied = CountUnsatisfied();
}

FStateNode::FStateNode(const TSharedRef<FWorldState>& WorldState, const TSharedPtr<FStateNode>& Node) :
	CurrentState(WorldState),
	GoalState(Node->GoalState),
	ParentNode(Node),
	ParentEdge(nullptr),
	PropFlags(Node->PropFlags),
	ForwardCost(Node->GetForwardCost()),
	NumUnsatisfied(Node->NumUnsatisfied),
	Depth(Node->Depth + 1),
	Closed(false)
{

}

int FStateNode::Cost() const 
{
	return ForwardCost + NumUnsatisfied;
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

bool FStateNode::IsClosed()
{
	return Closed;
}

void FStateNode::SetUnsatisfied(EWorldKey Key)
{

}
int FStateNode::GetForwardCost()
{
	return ForwardCost;
}

bool FStateNode::IsGoal() 
{
	return NumUnsatisfied <= 0;
}

void FStateNode::GetNeighboringEdges(const LookupTable& ActionMap, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions)
{
	if (!GoalState)
	{
		return;
	}
	const auto& Container = CurrentState->expose_container();
	for (uint8 Key = 0U; Key < (uint8)EWorldKey::SYMBOL_MAX; ++Key)
	{
		if (!CurrentState->IsSatisfied((EWorldKey)Key))
		{
			ActionMap.MultiFind((EWorldKey)Key, out_actions);
		}
	}
}

TSharedPtr<FStateNode> FStateNode::CreateStartNode(const TArray<FWorldProperty>& GoalSet, const TSharedPtr<FWorldState>& InitialState)
{
	return TSharedPtr<FStateNode>();
}

TSharedPtr<FStateNode> FStateNode::GenerateNeighbor(const TSharedPtr<FStateNode>& CurrentNode, UGOAPAction* Action)
{
	TSharedPtr<FStateNode> ChildNode(new FStateNode(CurrentNode->CurrentState->Clone(), CurrentNode));
	if (!ChildNode.IsValid())
	{
		return nullptr;
	}
	if (!ChildNode->ChainBackward(Action))
	{
		return nullptr;
	}
	//Actually generate the neighbor
	return ChildNode;
}

void FStateNode::LogNode() const
{
	if (ParentEdge)
	{
		UE_LOG(LogWS, Warning, TEXT("Parent edge: %.8s"), *ParentEdge->GetName());
	}
	else
	{
		UE_LOG(LogWS, Warning, TEXT("Node (start)"));
	}
	CurrentState->LogWS(GoalState.Get());
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

}

bool FStateNode::ChainBackward(UGOAPAction* Action)
{

	ParentEdge = Action;
	//resolve worldstate
	Action->UnapplySymbolicEffects(this);

	//Add preconditions and determine if satisfied
	Action->AddUnsatisfiedPreconditions(this);

	CurrentState->CacheArrayTypeHash();
	NumUnsatisfied = CountUnsatisfied();

	//add cost of action to produce current total forward cost
	ForwardCost += Action->Cost();
	return true;
}

void FStateNode::UnapplyProperty(const FWorldProperty& Property)
{
	if (Property.DataType == FWorldProperty::Type::kVariable)
	{
		//I'm not sure if this should be different in the inverse application
		//this isn't even correct lol it the Key should be the property value
		CurrentState->ApplyFromOther(GoalState.Get(), Property.Key);
	}
	else
	{
		//This is correct
		CurrentState->ApplyFromOther(GoalState.Get(), Property.Key);
	}
	CurrentState->ValidateProperty(GoalState.Get(), Property.Key);

}

void FStateNode::AddPrecondition(const FWorldProperty& Property)
{

	//Variable : lookup value in other prop 
	//(non-recursive, props in all world states are assumed const)
	if (Property.DataType == FWorldProperty::Type::kVariable && ParentNode.IsValid())
	{
		if (TSharedPtr<FStateNode> ParentShared = ParentNode.Pin())
		{
			FWorldProperty PropCopy(ParentShared->CurrentState->GetProperty((EWorldKey)Property.nValue));
			PropCopy.Key = Property.Key;
			CurrentState->Apply(PropCopy);
		}
	}
	else
	{
		CurrentState->Apply(Property);
	}
	CurrentState->ValidateProperty(GoalState.Get(), Property.Key);
}

int FStateNode::CountUnsatisfied()
{
	int iNumUnsatisfied = 0;
	auto& Container = CurrentState->expose_container(); //should change this now that all WS contain all props
	for (auto& Property : Container) {
		//if the prop is unsatisfied, find an action

		if (Property.bUnsatisfied)
		{
			PropFlags[(uint8)Property.Key] = true;
			++iNumUnsatisfied;
		}
	}

	return iNumUnsatisfied;
}