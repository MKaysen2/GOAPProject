#include "..\Public\WorldState.h"

void FWorldState::Add(const FWorldProperty& Prop)
{
	State.Add(Prop);
}

bool FWorldState::Apply(const FWorldProperty& Prop) 
{

	int32 Idx = State.IndexOfByKey(Prop);
	if (Idx == INDEX_NONE)
	{
		return false;
	}
	State[Idx].bValue = Prop.bValue;
	return true;
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
	State[NewIdx].MarkSatisfied((Property.bValue == Other->State[Idx].bValue));
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
	State[Idx].bValue = Other->State[OtherIdx].bValue;
	State[Idx].MarkSatisfied(true);
	return true;
}

bool FWorldState::Satisfied(const FWorldProperty& property) const
{
	int32 Index =  State.IndexOfByKey(property);
	if(Index == INDEX_NONE)
		return false;
	return (State[Index].bValue == property.bValue);
}

FWorldState* FWorldState::Clone()
{
	return (new FWorldState(*this));
}