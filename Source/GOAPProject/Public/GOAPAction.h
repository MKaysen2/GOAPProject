#pragma once

#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "Containers/Array.h"
#include "UObject/NoExportTypes.h"

#include "UObject/ConstructorHelpers.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "GOAPAction.generated.h"

class UWorldState;
class AAIController;

DECLARE_DELEGATE( FActionEndedDelegate );

//Analogous to FSM States
//State transitions are not explicitly defined, instead
//they are computed by solving a symbolic world representation
UCLASS(ABSTRACT, BlueprintType, Blueprintable)
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
	UFUNCTION()
	virtual bool VerifyContext(AAIController* Controller) 
	{
		//prevent accidental base class instances from being valid
		return false;
	}

	//forward application of action
	//Call after validating action success
	//TODO: Variable-valued symbols
	UFUNCTION()
	void ApplySymbolicEffects(UWorldState* state) const;

	//reverse application of action
	//Returns number of successfully satisfied properties
	UFUNCTION()
	virtual int UnapplySymbolicEffects(UWorldState* CurrentState, const UWorldState* GoalState) const;

	FActionEndedDelegate OnActionEnded;

	UFUNCTION()
	virtual bool IsActionRunning();
	UFUNCTION()
	virtual void StartAction(AAIController* controller);
	UFUNCTION()
	virtual void StopAction(AAIController* controller); //this might not be necessary
};


UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_MoveTo : public UGOAPAction 
{
	GENERATED_BODY()
private:

public:
	UAIAct_MoveTo();
	
	bool VerifyContext(AAIController* controller) override;
	void StartAction(AAIController * controller) override;
};

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Equip : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Equip();
	bool VerifyContext(AAIController* controller) override 
	{
		//Check blackboard for equippable weapon
		return true;
	}
	void StartAction(AAIController* Controller) override;
};

typedef TMultiMap<FWorldProperty, UGOAPAction*> LookupTable;