#pragma once

#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "WorldState.h"
#include "AIController.h"
#include "Containers/Array.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "Delegates/Delegate.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "GOAPAction.generated.h"

//Analogous to FSM States
//State transitions are not explicitly defined, instead
//they are computed by solving a symbolic world representation
UCLASS(ABSTRACT)
class GOAPPROJECT_API UGOAPAction : public UObject 
{
	GENERATED_BODY()
public:
	UGOAPAction();
	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> preconditions;

	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> effects;
	UPROPERTY()
		int edge_cost; 

	//Would not need this for singleton, but may want AI-specific context. Decisions...
	UPROPERTY()
	bool bIsRunning = false;

	virtual int cost() const 
	{
		return edge_cost;
	}
	
	//Try to access cached values here rather than perform direct computation
	virtual bool verify_context(AAIController* Controller) 
	{
		return false;
	}

	//forward application of action
	//Call after validating action success
	//TODO: Variable-valued symbols
	virtual void apply_symbolic_effects(UWorldState* state) const 
	{
		for (auto& prop : effects) 
		{
			state->apply_effect(prop);
		}
	}

	//Redundant, essentially what happens when you run the action itself
	virtual void apply_procedural_effects(AAIController* controller);

	//reverse application of action
	//Returns number of successfully satisfied properties
	virtual int unapply_action(UWorldState* CurrentState, const UWorldState* GoalState) const 
	{
		//TODO: solve variable properties by getting value indicated by prop
		int satisfied = 0;
		for (auto& prop : effects) 
		{
			const FWorldProperty* prev_prop = GoalState->find_property(prop);
			check(prev_prop); //sanity check - the controller state should define all of the symbols
			if (CurrentState->apply_effect(*prev_prop))
				++satisfied;
		}
		return satisfied;
	}

	UFUNCTION()
	virtual bool IsActionRunning();
	UFUNCTION()
	virtual void start_action(AAIController* controller);
	UFUNCTION()
	virtual void stop_action(AAIController* controller); //this might not be necessary
};


UCLASS(BlueprintType)
class GOAPPROJECT_API UActionMoveTo : public UGOAPAction 
{
	GENERATED_BODY()
private:
	UPROPERTY()
	int times_run;

public:
	UActionMoveTo();
	
	bool verify_context(AAIController* controller) override;
	void start_action(AAIController * controller) override;
};

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Reload : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Reload();

	bool verify_context(AAIController* controller) override;
	void start_action(AAIController* controller) override;
	
};

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Equip : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Equip();
	bool verify_context(AAIController* controller) override 
	{
		//Check blackboard for equippable weapon
		return true;
	}
	void start_action(AAIController* Controller) override;
};

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Attack : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Attack();
	bool verify_context(AAIController* controller) override
	{
		//Check BB for visible target
		return true;
	}
	void start_action(AAIController* Controller) override;
};
typedef TMultiMap<FWorldProperty, UGOAPAction*> LookupTable;