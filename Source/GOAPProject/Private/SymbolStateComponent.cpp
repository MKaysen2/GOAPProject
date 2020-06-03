#include "..\Public\SymbolStateComponent.h"

USymbolStateComponent::USymbolStateComponent() : Super(), WorldState(MakeShared<FWorldState>())
{
}

uint8 USymbolStateComponent::GetProp(const EWorldKey& Key) const
{
	return WorldState->GetProp(Key);
}

void USymbolStateComponent::SetProp(const EWorldKey& Key, const uint8& Value)
{
	WorldState->SetProp(Key, Value);
}
