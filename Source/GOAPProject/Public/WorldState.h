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

public:
	FWorldState();
	void Add(const FWorldProperty& prop);
	bool Apply(const FWorldProperty& property);
	bool ApplyFromOther(const FWorldState* Other, EWorldKey Key);
	void ValidateProperty(const FWorldState* Other, EWorldKey Key);

	//redundant, should combine
	//checks flag on struct so other state is not passed in
	bool Satisfied(const FWorldProperty& prop) const;
	bool IsSatisfied(EWorldKey Key) const;
	const FWorldProperty& GetProperty(EWorldKey Key);
	const TArray<FWorldProperty>& expose_container()const { //i don't want to but i feel like i have to
		return State;
	}

	uint32 GetArrayTypeHash()
	{
		return GetTypeHash(State);
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
	FWorldState* Clone();

	void LogWS(const FWorldState* Other = nullptr) const;
#if WITH_GAMEPLAY_DEBUGGER
	void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};

