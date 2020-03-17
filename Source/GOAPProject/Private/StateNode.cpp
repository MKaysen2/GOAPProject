#include "..\Public\StateNode.h"
#include "..\Public\WorldState.h"

FStateNode::FStateNode() :
	CurrentState(),
	GoalState(),
	ParentNode(),
	ParentEdge(),
	forward_cost(0),
	unsatisfied(0),
	Depth(0)
{
	UE_LOG(LogTemp, Warning, TEXT("FStateNode default constructor"));
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
		CurrentState->AddPropertyAndTrySatisfy(GoalState.Get(), Property);
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


void FStateNode::FindActions(const LookupTable& action_map, TArray<UGOAPAction*>& out_actions)
{
	if (!GoalState)
		return;
	auto& Container = CurrentState->expose_container();
	for (auto Property : Container) {
		//if the prop is unsatisfied, find an action
		if (!Property.bSatisfied) 
		{
			action_map.MultiFind(Property.key, out_actions);
		}
	}
}

void FStateNode::TakeAction(const UGOAPAction* Action) 
{
	if (!CurrentState.IsValid() || !GoalState.IsValid())
	{
		return;
	}
	Action->UnapplySymbolicEffects(*CurrentState, *GoalState);
	
	Action->AddUnsatisfiedPreconditions(*CurrentState, *GoalState);

	unsatisfied = CountUnsatisfied();
	
	//add cost of action to produce current total forward cost
	forward_cost += Action->Cost();
}

int FStateNode::CountUnsatisfied()
{
	int iNumUnsatisfied = 0;
	auto& Container = CurrentState->expose_container();
	for (auto& Property : Container) {
		//if the prop is unsatisfied, find an action

		if (!Property.bSatisfied)
		{
			++iNumUnsatisfied;
		}
	}

	return iNumUnsatisfied;
}