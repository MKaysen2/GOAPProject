#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "Containers/StaticArray.h"
#include "Hash/CityHash.h"

#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.generated.h"

struct FStateNode;
class FGameplayDebuggerCategory;

DECLARE_LOG_CATEGORY_EXTERN(LogWS, Warning, All);

USTRUCT()
struct GOAPPROJECT_API FWorldState
{
	
	GENERATED_BODY()

private: 
		TStaticArray<uint8, (uint32)EWorldKey::SYMBOL_MAX> State;

public:
	FWorldState() = default;
	FWorldState(const FWorldState& Copy) = default;

	void SetProp(EWorldKey Key, uint8 Value);

	const uint8& GetProp(EWorldKey Key) const;
	
	bool CheckCondition(const FWorldProperty& Condition) const;

	void SatisfyCondition(const FWorldProperty& Condition);

	bool ApplyEffect(const FAISymEffect& Effect);

	bool RevertEffect(const FAISymEffect& Effect);

	uint32 Num() const
	{
		return State.Num();
	}
	//Hamming distance by default, o.w. the absolute value of the difference
	//should make this an int32
	int32 HeuristicDist(EWorldKey Key, uint8 Value, bool bHamming=true) const;

	friend FORCEINLINE uint32 GetTypeHash(const FWorldState& WorldState)
	{
		return CityHash32((const char*)WorldState.State.GetData(), WorldState.Num());
	}

	void LogWS() const;
#if WITH_GAMEPLAY_DEBUGGER
	void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};

