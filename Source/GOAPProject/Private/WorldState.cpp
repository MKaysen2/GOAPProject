#include "..\Public\WorldState.h"

FWorldState::FWorldState()
{
	State.Reserve((int32)EWorldKey::SYMBOL_MAX);
	for (uint8 Key = 0; Key < (uint8)EWorldKey::SYMBOL_MAX; ++Key)
	{
		State.Emplace(FWorldProperty((EWorldKey)Key, false));
	}
}

void FWorldState::Add(const FWorldProperty& Prop)
{
	uint8 Key = (uint8)Prop.key;
	FWorldProperty& Property = State[Key];
	Property.Apply(Prop);
	Property.bUnsatisfied = true;
}

bool FWorldState::Apply(const FWorldProperty& Prop) 
{

	uint8 Key = (uint8)Prop.key;
	State[Key].Apply(Prop);
	State[Key].bUnsatisfied = false;
	return true;
}


void FWorldState::AddPropertyAndTrySatisfy(const FWorldState* Other, FWorldProperty Property)
{
	
	uint8 Key = (uint8)Property.key;
	Add(Property);
	bool bEquals = Other->State[Key].Equals(Property);
	State[Key].MarkSatisfied(bEquals);
}

bool FWorldState::TrySatisfyPropertyFrom(const FWorldState* Other, const FWorldProperty& Property)
{

	if (!Other)
	{
		return false;
	}

	uint8 Key = (uint8)Property.key;
	State[Key].Apply(Other->State[Key]);
	State[Key].MarkSatisfied(true);
	return true;
}

bool FWorldState::Satisfied(const FWorldProperty& Property) const
{
	int Idx = (int32)Property.key;
	return !(State[Idx].bUnsatisfied);
}

bool FWorldState::IsSatisfied(EWorldKey Key) const
{
	int Idx = (int32)Key;
	return !(State[Idx].bUnsatisfied);
}

FWorldState* FWorldState::Clone()
{
	return (new FWorldState(*this));
}