#include "..\Public\StateNode.h"
#include "..\Public\WorldState.h"
#include "..\Public\GOAPAction.h"

FStateNode::FStateNode() :
	CurrentState(),
	GoalState(),
	ParentNode(),
	ParentEdge(),
	forward_cost(0),
	unsatisfied(0),
	Depth(0)
{
	//UE_LOG(LogTemp, Warning, TEXT("FStateNode default constructor"));
	//Not sure this is ever used
}

FStateNode::FStateNode(const TArray<FWorldProperty>& GoalSet, TSharedPtr<FWorldState> InitialState) :
	CurrentState(),
	GoalState(InitialState),
	ParentNode(),
	ParentEdge(),
	forward_cost(0),
	unsatisfied(0),
	Depth(0)
{
	CurrentState = MakeShared<FWorldState>();
	for (auto Property : GoalSet)
	{
		CurrentState->Add(Property);
		CurrentState->ValidateProperty(GoalState.Get(), Property.key);
	}
	unsatisfied = CountUnsatisfied();
}

FStateNode::FStateNode(TSharedPtr<FStateNode> Node, UGOAPAction* Edge) :
	CurrentState(Node->CurrentState->Clone()),
	GoalState(Node->GoalState),
	ParentNode(Node),
	ParentEdge(Edge),
	forward_cost(Node->cost()),
	unsatisfied(Node->unsatisfied),
	Depth(Node->Depth + 1)
{
}

int FStateNode::cost() const 
{
	return forward_cost + unsatisfied;
}

bool FStateNode::IsGoal() 
{
	return unsatisfied <= 0;
}

void FStateNode::FindActions(const LookupTable& ActionMap, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions)
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

void FStateNode::TakeAction(const UGOAPAction* Action) 
{
	if (!CurrentState.IsValid() || !GoalState.IsValid())
	{
		return;
	}

	//resolve worldstate
	Action->UnapplySymbolicEffects(this);

	//Add preconditions and determine if satisfied
	Action->AddUnsatisfiedPreconditions(this);

	unsatisfied = CountUnsatisfied();
	
	//add cost of action to produce current total forward cost
	forward_cost += Action->Cost();
}

void FStateNode::UnapplyProperty(const FWorldProperty& Property)
{
	if (Property.DataType == FWorldProperty::Type::kVariable)
	{
		//I'm not sure if this should be different in the inverse application
		CurrentState->ApplyFromOther(GoalState.Get(), Property.key);
	}
	else
	{
		//This is correct
		CurrentState->ApplyFromOther(GoalState.Get(), Property.key);
	}
	CurrentState->ValidateProperty(GoalState.Get(), Property.key);

}

void FStateNode::AddPrecondition(const FWorldProperty& Property)
{

	//Variable : lookup value in other prop 
	//(non-recursive, props in all world states are assumed const)
	if (Property.DataType == FWorldProperty::Type::kVariable && ParentNode.IsValid())
	{
		FWorldProperty PropCopy(ParentNode->CurrentState->GetProperty(Property.Data.kValue));
		PropCopy.key = Property.key;
		CurrentState->Apply(PropCopy);
	}
	else
	{
		CurrentState->Apply(Property);
	}
	CurrentState->ValidateProperty(GoalState.Get(), Property.key);
}

int FStateNode::CountUnsatisfied()
{
	int iNumUnsatisfied = 0;
	auto& Container = CurrentState->expose_container(); //should change this now that all WS contain all props
	for (auto& Property : Container) {
		//if the prop is unsatisfied, find an action

		if (Property.bUnsatisfied)
		{
			++iNumUnsatisfied;
		}
	}

	return iNumUnsatisfied;
}