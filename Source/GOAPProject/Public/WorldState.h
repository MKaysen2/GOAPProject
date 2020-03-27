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

	//True when prop is element in state and state's value for key matches prop's value
	bool Satisfied(const FWorldProperty& prop) const;
	bool IsSatisfied(EWorldKey Key) const;

	const TArray<FWorldProperty>& expose_container()const { //i don't want to but i feel like i have to
		return State;
	}

	FWorldState* Clone();

	void LogWS() const;
#if WITH_GAMEPLAY_DEBUGGER
	void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};

