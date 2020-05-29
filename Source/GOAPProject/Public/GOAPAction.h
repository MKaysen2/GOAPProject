#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.h"
#include "UObject/ConstructorHelpers.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "GOAPAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAction, Warning, All);

class AAIController;
struct FStateNode;
struct FAIRequestID;
struct FPathFollowingResult;
struct FWorldState;
DECLARE_DELEGATE( FActionEndedDelegate );

UENUM(BlueprintType)
enum class EActionStatus : uint8
{
	kFailed,
	kRunning,
	kSuccess
};

//Analogous to FSM States
//State transitions are not explicitly defined, instead
//they are computed by solving a symbolic world representation
//TODO: just add the controlled pawn as a property already, seriously
//TODO: Make this not reference stateNode at all, just WorldState
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

	UPROPERTY()
		AAIController* AIOwner;

	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> Preconditions;

	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> Effects;

	//If the union member prevents us from correctly using FWorldProperty
	//in Blueprints, we should subclass it and add a map of EWorldKey to TUnion
	//and use that to construct BP subclasses

	UPROPERTY()
	int EdgeCost;

	UPROPERTY()
	bool bIsRunning = false;

	//Should add effects and preconditions in InitAction or something
	//which will make it easier to create BP subclasses
public:

	UFUNCTION()
		const TArray<FWorldProperty>& GetEffects() const 
	{
		return Effects;
	}

	UFUNCTION()
	virtual int Cost() const 
	{
		return EdgeCost;
	}
	
	virtual void SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState> Context);
	//Try to access cached values here rather than perform direct computation

	/**VerifyContext
	  * Used to verify context preconditions and cache data dependencies
	  * By default, returns false so must be overridden
	  * @return bool whether Action can be run
	  */
	UFUNCTION()
	virtual bool VerifyContext()
	{
		//prevent accidental base class instances from being valid
		return false;
	}

	//forward application of action
	//I don't think this actually needs to be called, might remove it
	//as the actions context effects should probably be picked up by the sensors
	UFUNCTION()
	void ApplySymbolicEffects(FWorldState& State) const;

	//reverse application of action
	//Returns number of successfully satisfied properties
	virtual void UnapplySymbolicEffects(FStateNode* CurrentNode) const;

	void AddUnsatisfiedPreconditions(FStateNode* CurrentNode) const;
	FActionEndedDelegate OnActionEnded;

	//Should be called when actions are created
	//I wanted to put it in a static method but
	//Templates don't work with TSubclassOf AFAIK
	UFUNCTION()
		virtual void InitAction(AAIController* Controller);
	UFUNCTION()
	virtual bool IsActionRunning();
	UFUNCTION()
	virtual EActionStatus StartAction();
	UFUNCTION()
	virtual void StopAction(); 
	
	/*Deactivates action, stops all child tasks, and unbind delegates*/
	//TODO: add an abort type to control blending
	//e.g. damage reactions require very fast blend out times
	UFUNCTION()
		virtual void AbortAction();
};

typedef TMultiMap<EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;