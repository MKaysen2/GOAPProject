#include "..\Public\WorldState.h"
#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerTypes.h"
#include "GameplayDebuggerCategory.h"
#endif

DEFINE_LOG_CATEGORY(LogWS);

void FWorldState::SetProp(EWorldKey Key, uint8 nValue) 
{
	uint8 Idx = (uint8)Key;
	State[Idx] = nValue;
}

const uint8& FWorldState::GetProp(EWorldKey Key) const
{
	uint8 Idx = (uint8)Key;
	return State[Idx];
}

int32 FWorldState::HeuristicDist(EWorldKey Key, uint8 Value, bool bHamming) const
{
	int32 StateVal = State[(uint8)Key];
	if (bHamming)
	{
		return StateVal != Value;
	}
	else
	{
		return abs(StateVal - Value);
	}
}

void FWorldState::LogWS() const
{
	uint8 Key = 0;
	for (const auto& Value : State)
	{
		UE_LOG(LogTemp, Warning, TEXT("< k%d | v%d >"), Key, Value);
		++Key;
	}
}

#if WITH_GAMEPLAY_DEBUGGER

void FWorldState::DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const
{
	DebuggerCategory->AddTextLine(FString(TEXT("WorldState")));

	uint8 Key = 0;
	for (const auto& Value : State)
	{
		FString PropText = FString::Printf(TEXT("< k%d | v%d >"), Key, Value);
		DebuggerCategory->AddTextLine(PropText);
		++Key;
	}
}
#endif //WITH_GAMEPLAY_DEBUGGER