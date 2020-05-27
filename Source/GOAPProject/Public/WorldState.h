#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.generated.h"

struct FStateNode;
class FGameplayDebuggerCategory;

DECLARE_LOG_CATEGORY_EXTERN(LogWS, Warning, All);

USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldState
{
	
	GENERATED_BODY()

private: 
	UPROPERTY(VisibleAnywhere)
		TArray<FWorldProperty> State;

	uint32 CachedTypeHash;
public:
	FWorldState();
	void Add(const FWorldProperty& prop);
	bool Apply(const FWorldProperty& property);
	bool ApplyFromOther(const FWorldState* Other, EWorldKey Key);
	void ValidateProperty(const FWorldState* Other, EWorldKey Key);

	bool IsSatisfied(EWorldKey Key) const;
	const FWorldProperty& GetProperty(EWorldKey Key);
	const TArray<FWorldProperty>& expose_container()const { //i don't want to but i feel like i have to
		return State;
	}

	void CacheArrayTypeHash()
	{
		//Should cache this result
		CachedTypeHash = GetTypeHash(State);
	}

	uint32 GetArrayTypeHash()
	{
		return CachedTypeHash;
	}
	bool EqualsTest(const FWorldState* Other, EWorldKey Key) const
	{
		if (!Other)
		{
			return false;
		}
		uint8 Idx = (uint8)Key;
		return State[Idx].Equals(Other->State[Idx]);
	}
	TSharedPtr<FWorldState> Clone();

	void LogWS(const FWorldState* Other = nullptr) const;
#if WITH_GAMEPLAY_DEBUGGER
	void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};

