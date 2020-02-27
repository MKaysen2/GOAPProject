#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.generated.h"

UCLASS(BlueprintType)
class GOAPPROJECT_API UWorldState : public UObject {
	
	GENERATED_BODY()

private: 
	UPROPERTY(VisibleAnywhere)
		TArray<FWorldProperty> state;
public:
	void AddPropertyAndTrySatisfy(const UWorldState* Other, FWorldProperty Property);
	void add_property(const FWorldProperty& prop);
	bool apply_effect(const FWorldProperty& property);
	bool TrySatisfyPropertyFrom(const UWorldState* Other, const FWorldProperty& Property);
	//True when prop is element in state and state's value for key matches prop's value
	bool satisfied(const FWorldProperty& prop) const;
	const TArray<FWorldProperty>& expose_container()const { //i don't want to but i feel like i have to
		return state;
	}
};

