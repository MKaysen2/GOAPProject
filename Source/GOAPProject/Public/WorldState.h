#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.generated.h"

USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldState
{
	
	GENERATED_BODY()

private: 
	UPROPERTY(VisibleAnywhere)
		TArray<FWorldProperty> State;

public:
	FWorldState();
	void AddPropertyAndTrySatisfy(const FWorldState* Other, FWorldProperty Property);
	void Add(const FWorldProperty& prop);
	bool Apply(const FWorldProperty& property);
	bool TrySatisfyPropertyFrom(const FWorldState* Other, const FWorldProperty& Property);
	//True when prop is element in state and state's value for key matches prop's value
	bool Satisfied(const FWorldProperty& prop) const;
	bool IsSatisfied(EWorldKey Key) const;

	const TArray<FWorldProperty>& expose_container()const { //i don't want to but i feel like i have to
		return State;
	}

	FWorldState* Clone();
};

