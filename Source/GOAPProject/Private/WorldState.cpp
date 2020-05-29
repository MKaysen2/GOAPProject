#include "..\Public\WorldState.h"
#include "Hash/CityHash.h"
#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerTypes.h"
#include "GameplayDebuggerCategory.h"
#endif

DEFINE_LOG_CATEGORY(LogWS);

FWorldState::FWorldState()
{
	State.Init(0, (int32)EWorldKey::SYMBOL_MAX);
}

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

uint8 FWorldState::HeuristicDist(EWorldKey Key, uint8 Value, bool bHamming) const
{
	const uint8& StateVal = State[(uint8)Key];
	return (bHamming) ? 
		(StateVal != Value) :
		((StateVal >= Value) ?
			(StateVal - Value) : (Value - StateVal));
}

void FWorldState::CacheArrayTypeHash()
{
	//Should cache this result
	CachedTypeHash = CityHash32((const char*)State.GetData(), State.Num());
}

void FWorldState::LogWS() const
{
	uint8 Key = 0;
	for (const auto& Value : State)
	{
		UE_LOG(LogTemp, Warning, TEXT("< k%d | v%d >"), Key, (uint8)Value);
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
		FString PropText = FString::Printf(TEXT("< k%d | v%d >"), Key, (uint8)Value);
		DebuggerCategory->AddTextLine(PropText);
		++Key;
	}
}
#endif //WITH_GAMEPLAY_DEBUGGER