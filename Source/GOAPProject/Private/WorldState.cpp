#include "..\Public\WorldState.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerTypes.h"
#include "GameplayDebuggerCategory.h"
#endif

DEFINE_LOG_CATEGORY(LogWS);

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

void FWorldState::ValidateProperty(const FWorldState* Other, EWorldKey Key)
{
	uint8 eKey = (uint8)Key;
	if (!Other)
	{
		return;
	}
	bool bEquals = State[eKey].Equals(Other->State[eKey]);
	State[eKey].MarkSatisfied(bEquals);
}

bool FWorldState::ApplyFromOther(const FWorldState* Other, EWorldKey eKey)
{

	if (!Other)
	{
		return false;
	}

	uint8 Key = (uint8)eKey;
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

void FWorldState::LogWS() const
{
	for (auto& Property : State)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Property.ToString());
	}
}

#if WITH_GAMEPLAY_DEBUGGER

void FWorldState::DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const
{
	DebuggerCategory->AddTextLine(FString(TEXT("WorldState")));

	for (auto& Property : State)
	{
		DebuggerCategory->AddTextLine(Property.ToString());
	}
}

#endif //WITH_GAMEPLAY_DEBUGGER