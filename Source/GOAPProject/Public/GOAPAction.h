#pragma once

#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "Containers/Array.h"
#include "WorldState.h"
#include "UObject/NoExportTypes.h"

#include "UObject/ConstructorHelpers.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "GOAPAction.generated.h"

class AAIController;
struct FStateNode;
struct FAIRequestID;
struct FPathFollowingResult;
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
protected:
	//callable by child actions only (i.e., in the constructor)
	//use this to construct child actions in the initializer list directly
	explicit UGOAPAction(TArray<FWorldProperty>&& Pre, TArray<FWorldProperty>&& Post, int _Cost);

public:
	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> preconditions;

	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> effects;
	UPROPERTY()
		int edge_cost; 

	//Would not need this for singleton, but may want AI-specific context. Decisions...
	UPROPERTY()
	bool bIsRunning = false;

	UFUNCTION()
	virtual int Cost() const 
	{
		return edge_cost;
	}
	
	virtual void SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState> Context);
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
	void ApplySymbolicEffects(FWorldState& State) const;

	//reverse application of action
	//Returns number of successfully satisfied properties
	//TODO: Variable-Valued symbols
	virtual void UnapplySymbolicEffects(FStateNode* CurrentNode) const;

	void AddUnsatisfiedPreconditions(FStateNode* CurrentNode) const;
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
protected:
	FDelegateHandle MoveHandle;
public:
	UAIAct_MoveTo();
	void SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState> Context) override;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result, AAIController* Controller);
	
	bool VerifyContext(AAIController* controller) override;
	void StartAction(AAIController* controller) override;
	void StopAction(AAIController* Controller) override;

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
	void SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState> Context) override;
	void StartAction(AAIController* Controller) override;
	void StopAction(AAIController* Controller) override;
};

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_ReactDisturbance : public UGOAPAction
{
	GENERATED_BODY()

		FTimerHandle TimerHandle;
public:
	UAIAct_ReactDisturbance();
	bool VerifyContext(AAIController* Controller) override;
	void StartAction(AAIController* Controller) override;
	void StopAction(AAIController* Controller) override;
};

//wait 3 seconds before ending
//for testing action callbacks without the planner, so no symbols defined
UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_CallbackTest : public UGOAPAction
{
	GENERATED_BODY()
		FTimerHandle TimerHandle;

public:
	UAIAct_CallbackTest();
	bool VerifyContext(AAIController* Controller) override;
	void StartAction(AAIController* Controller) override;
	void StopAction(AAIController* Controller) override;
};

typedef TMultiMap<EWorldKey, UGOAPAction*> LookupTable;