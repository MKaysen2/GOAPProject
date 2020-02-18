#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "IAStarNode.h"
#include "WorldState.generated.h"

UCLASS(BlueprintType)
class GOAPPROJECT_API UWorldState : public UObject {
	
	GENERATED_BODY()

private: 
	UPROPERTY(EditAnywhere)
		TSet<FWorldProperty> state;
public:
	void add_property(const FWorldProperty& prop);
	bool apply_effect(const FWorldProperty& property); //returns whether the property was actually found 
	const FWorldProperty* find_property(const FWorldProperty& prop) const;
	//True when prop is element in state and state's value for key matches prop's value
	bool satisfied(const FWorldProperty& prop) const;
	const TSet<FWorldProperty>& expose_container()const { //i don't want to but i feel like i have to
		return state;
	}
};

