#include "..\Public\WorldState.h"

void FWorldState::Add(const FWorldProperty & Prop)
{
	State.Add(Prop);
}

bool FWorldState::Apply(const FWorldProperty& Prop) {
	
	return false;
}

void FWorldState::AddPropertyAndTrySatisfy(const FWorldState* Other, FWorldProperty Property)
{
	if (!Other)
	{
		return;
	}
	int32 Idx = Other->State.IndexOfByKey(Property);
	if (Idx == INDEX_NONE)
	{
		return;
	}
	int32 NewIdx = State.Add(Property);
	State[NewIdx].MarkSatisfied((Property.value == Other->State[Idx].value));
}
bool FWorldState::TrySatisfyPropertyFrom(const FWorldState* Other, const FWorldProperty& Property)
{

	if (!Other)
	{
		return false;
	}
	int32 Idx = State.IndexOfByKey(Property);
	int32 OtherIdx = Other->State.IndexOfByKey(Property);
	if (Idx == INDEX_NONE || OtherIdx == INDEX_NONE)
	{
		return false;
	}
	State[Idx].value = Other->State[OtherIdx].value;
	State[Idx].MarkSatisfied(true);
	return true;
}

bool FWorldState::Satisfied(const FWorldProperty& property) const
{
	int32 Index =  State.IndexOfByKey(property);
	if(Index == INDEX_NONE)
		return false;
	return (State[Index].value == property.value);
}

FWorldState* FWorldState::Clone()
{
	return (new FWorldState(*this));
}