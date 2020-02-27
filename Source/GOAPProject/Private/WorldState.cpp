#include "..\Public\WorldState.h"

void UWorldState::add_property(const FWorldProperty & prop)
{
	state.Add(prop);
}

bool UWorldState::apply_effect(const FWorldProperty& property) {
	
	return false;
}

void UWorldState::AddPropertyAndTrySatisfy(const UWorldState* Other, FWorldProperty Property)
{
	if (!Other)
	{
		return;
	}
	int32 Idx = Other->state.IndexOfByKey(Property);
	if (Idx == INDEX_NONE)
	{
		return;
	}
	int32 NewIdx = state.Add(Property);
	state[NewIdx].MarkSatisfied((Property.value == Other->state[Idx].value));
}
bool UWorldState::TrySatisfyPropertyFrom(const UWorldState* Other, const FWorldProperty& Property)
{

	if (!Other)
	{
		return false;
	}
	int32 Idx = state.IndexOfByKey(Property);
	int32 OtherIdx = Other->state.IndexOfByKey(Property);
	if (Idx == INDEX_NONE || OtherIdx == INDEX_NONE)
	{
		return false;
	}
	state[Idx].value = Other->state[OtherIdx].value;
	state[Idx].MarkSatisfied(true);
	return true;
}

bool UWorldState::satisfied(const FWorldProperty& property) const
{
	int32 Index =  state.IndexOfByKey(property);
	if(Index == INDEX_NONE)
		return false;
	return (state[Index].value == property.value);
}
