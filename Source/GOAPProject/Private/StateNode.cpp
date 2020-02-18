#include "..\Public\StateNode.h"

FStateNode::FStateNode()
{
	CurrentState = NewObject<UWorldState>();
	GoalState = nullptr;
	ParentEdge = nullptr;
	ParentNode = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("FStateNode default constructor"));
}

FStateNode::FStateNode(TSharedPtr<FStateNode> Node, UGOAPAction* Edge) :
	CurrentState(nullptr),
	GoalState(Node->GoalState),
	ParentNode(Node),
	ParentEdge(Edge),
	unsatisfied(Node->unsatisfied),
	forward_cost(Node->cost())
{
	const UWorldState* ParentState = const_cast<const UWorldState*>(Node->CurrentState);
	CurrentState = DuplicateObject<UWorldState>(ParentState, ParentState->GetOuter());
}

void FStateNode::SetupInitialNode(const TArray<FWorldProperty>& goal, const UWorldState* ControllerState)
{
	if (!ControllerState) {
		return;
	}
	GoalState = ControllerState;
	for (auto& prop : goal) {
		CurrentState->add_property(prop);
		const FWorldProperty * lookup = GoalState->find_property(prop);
		check(lookup);
		if (!(prop.value == lookup->value))
		{
			++unsatisfied;
		}
	}
}

int FStateNode::cost() const {
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
	auto& desired = CurrentState->expose_container();
	for (auto prop : desired) {
		//if the prop is unsatisfied, find an action
		if (!GoalState->satisfied(prop)) 
		{
			action_map.MultiFind(prop, out_actions);
		}
	}
}

void FStateNode::TakeAction(const UGOAPAction* action) 
{

	int num_satisfied = action->unapply_action(CurrentState, GoalState);
	unsatisfied -= num_satisfied;
	//for every prop in preconditions
	for (auto prop : action->preconditions) 
	{
		//add the property to the goal state
		CurrentState->add_property(prop);

		//find the property in the Controller's world state 
		//add controller's prop to the node's world state
		const FWorldProperty* controller_prop = GoalState->find_property(prop);
		check(controller_prop); // the controllers state should have ALL properties, I think
		//Property is unsatisfied so increment counter
		if (!(prop.value == controller_prop->value))
			++unsatisfied;
	}
	//add cost of action to produce current total forward cost
	forward_cost += action->cost();
}