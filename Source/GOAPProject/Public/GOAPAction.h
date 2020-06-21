#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.h"
#include "Tasks/AITask.h"
#include "GameplayTaskOwnerInterface.h"

#include "GOAPAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAction, Warning, All);

class UAITask;
class UAITask_Operator;
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
UCLASS(Config=AI, EditInlineNew, meta=(DisplayName="Planner Primitive"))
class GOAPPROJECT_API UGOAPAction : public UObject, public IGameplayTaskOwnerInterface
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

	UPROPERTY()
		UPlannerComponent* OwnerComp;

	UPROPERTY(EditAnywhere)
		FString ActionName;
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
		
	UPROPERTY(EditAnywhere, Instanced)
		UAITask_Operator* Operator;

	UPROPERTY(transient)
		UAITask_Operator* OpInstance = nullptr;
public:

	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

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

	UFUNCTION()
		void SetOwner(AAIController* Controller, UPlannerComponent* OwnerComponent);
	virtual UAITask* GetOperator();

	FString GetActionName() const { return ActionName; };
	UFUNCTION()
	EActionResult StartAction();

	void FinishAction(EPlannerTaskFinishedResult::Type Result);
	
	void OnOperatorEnded();
	/*Deactivates action, stops all child tasks, and unbind delegates*/
	//TODO: add an abort type to control blending
	//e.g. damage reactions require very fast blend out times
	EActionResult AbortAction();

	template<typename T>
	T* NewOperatorTask(AAIController& Controller)
	{
		return UAITask::NewAITask<T>(Controller, *this);
	}
protected:
	//Should add effects and preconditions in InitAction or something
	//which will make it easier to create BP subclasses
	void AddEffect(const EWorldKey& Key, const FAISymEffect& Effect);

	void AddPrecondition(const EWorldKey& Key, const uint8& Value);

};

typedef TMultiMap<EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;