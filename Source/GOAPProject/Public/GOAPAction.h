#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.h"

#include "GOAPAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAction, Warning, All);

class UAITask;
class UAnimMontage;
class AAIController;
class UBrainComponent;
class UPlannerComponent;
struct FAIRequestID;
struct FPathFollowingResult;
struct FWorldState;
DECLARE_DELEGATE( FActionEndedDelegate );

UENUM(BlueprintType)
enum class EActionResult : uint8
{
	Failed,
	Aborted,
	Running,
	Success
};

UENUM()
enum class EActionStatus : uint8
{
	Active,
	Aborting
};


//Analogous to FSM States
//State transitions are not explicitly defined, instead
//they are computed by solving a symbolic world representation
UCLASS(Config=AI, abstract, EditInlineNew, config=Game)
class GOAPPROJECT_API UGOAPAction : public UObject 
{
	GENERATED_BODY()
public:
	UGOAPAction();

	//Message names, might move to BrainComponent
	static const FName MontageCompleted;
	static const FName MontageBlendingOut;
	static const FName ActionFinished;

protected:
	explicit UGOAPAction(const int& Cost);

	UPROPERTY()
		AAIController* AIOwner;

	//I don't know why I can't modify this in the editor
	//cause you didn't tag the properties, ya dingus
	UPROPERTY(config, EditAnywhere)
		TArray<FWorldProperty> Preconditions;

	UPROPERTY(config, EditAnywhere)
		TArray<FAISymEffect> Effects;

	UPROPERTY(EditDefaultsOnly)
	int EdgeCost;

	UPROPERTY()
		EActionStatus TaskStatus;
		//PawnAction has EditInlineNew defined so should be able to set this
		//in editor
	//this sort of works, but no ChildAction functionality as of right now
	//Since ChildAction isn't an instanced property of PawnAction
	UPROPERTY(transient)
		UAITask* Operator;

public:

	UFUNCTION()
		const TArray<FAISymEffect>& GetEffects() const 
	{
		return Effects;
	}

	UFUNCTION()
		const TArray<FWorldProperty>& GetPreconditions() const
	{
		return Preconditions;
	}

	UFUNCTION()
	virtual int Cost() const 
	{
		return EdgeCost;
	}

	//TODO: don't need to get rid of this, but shouldn't do it in the planner
	/**VerifyContext
	  * Used to verify context preconditions and cache data dependencies
	  * By default, returns false so must be overridden
	  * @return bool whether Action can be run
	  */
	UFUNCTION()
	virtual bool VerifyContext()
	{
		return true;
	}

	//Should be called when actions are created
	//Does not activate the action, just adds it to the controller
	//Make sure you call this if you want the actions to be in the planner!
	UFUNCTION()
		virtual void InitAction(AAIController* Controller);

	virtual UAITask* GetOperator();

	UFUNCTION()
	EActionResult StartAction();

	void FinishAction(EPlannerTaskFinishedResult::Type Result);
	
	void OnOperatorEnded();
	/*Deactivates action, stops all child tasks, and unbind delegates*/
	//TODO: add an abort type to control blending
	//e.g. damage reactions require very fast blend out times
	EActionResult AbortAction();

protected:
	//Should add effects and preconditions in InitAction or something
	//which will make it easier to create BP subclasses
	void AddEffect(const EWorldKey& Key, const FAISymEffect& Effect);

	void AddPrecondition(const EWorldKey& Key, const uint8& Value);

	/**
	 * Override to add action preconditions
	 */
	UFUNCTION()
		virtual void InitPreconditions();

	/**
	 * Override to add action effects
	 */
	UFUNCTION()
		virtual void InitEffects();
};

UCLASS(meta = (DisplayName = "Task primitive Animate"))
class GOAPPROJECT_API UAIAct_Animate : public UGOAPAction
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
		UAnimMontage* Montage;

	virtual UAITask* GetOperator() override;
};
typedef TMultiMap<EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;