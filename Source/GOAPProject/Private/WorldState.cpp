#include "..\Public\WorldState.h"

void UWorldState::add_property(const FWorldProperty & prop)
{
	state.Add(prop);
}

bool UWorldState::apply_effect(const FWorldProperty& property) {
	
	FWorldProperty * lookup = state.Find(property);
	if (!lookup)
		return false;
	lookup->value = property.value;
	return true;
}

const FWorldProperty* UWorldState::find_property(const FWorldProperty& prop) const {
	return state.Find(prop);
}

bool UWorldState::satisfied(const FWorldProperty& property) const
{
	const FWorldProperty * lookup = state.Find(property);
	if(!lookup)
		return false;
	return (lookup->value == property.value);
}
