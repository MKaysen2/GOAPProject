#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.h"
#include "Tasks/AITask.h"
#include "GameplayTaskOwnerInterface.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

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


//Analogous to FSM States
//State transitions are not explicitly defined, instead
//they are computed by solving a symbolic world representation
UCLASS(abstract, EditInlineNew, meta=(DisplayName="Planner Primitive"))
class GOAPPROJECT_API UGOAPAction : public UObject
{
	GENERATED_BODY()
public:
	UGOAPAction();
	UGOAPAction(const FObjectInitializer& ObjectInitializer);

	//Message names, might move to BrainComponent
	static const FName MontageCompleted;
	static const FName MontageBlendingOut;
	static const FName ActionFinished;

protected:

	UPROPERTY()
		AAIController* AIOwner;

	UPROPERTY()
		UPlannerComponent* OwnerComp;

	UPROPERTY(EditAnywhere)
		FString ActionName;
	//I don't know why I can't modify this in the editor
	//cause you didn't tag the properties, ya dingus
	UPROPERTY(EditAnywhere)
		TArray<FWorldProperty> Preconditions;
	
	UPROPERTY(EditAnywhere)
		TArray<FAISymEffect> Effects;

	UPROPERTY(EditDefaultsOnly)
	int EdgeCost;

	UPROPERTY()
		EActionStatus TaskStatus;

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

	bool ValidatePlannerPreconditions(const FWorldState& WorldState);
	//Should be called when actions are created
	//Does not activate the action, just adds it to the controller
	//Make sure you call this if you want the actions to be in the planner!
	UFUNCTION()
		virtual void InitAction(AAIController* Controller);

	UFUNCTION()
		void SetOwner(AAIController* Controller, UPlannerComponent* OwnerComponent);

	FString GetActionName() const { return ActionName; };
	UFUNCTION()
	virtual EActionResult StartAction();

	virtual void FinishAction(EPlannerTaskFinishedResult::Type Result);
	virtual void TickAction(float DeltaTime);
	/*Deactivates action, stops all child tasks, and unbind delegates*/
	//TODO: add an abort type to control blending
	//e.g. damage reactions require very fast blend out times
	virtual EActionResult AbortAction();

protected:
	virtual UWorld* GetWorld() const override;
	//Should add effects and preconditions in InitAction or something
	//which will make it easier to create BP subclasses
	void AddEffect(const EWorldKey& Key, const FAISymEffect& Effect);

	void AddPrecondition(const EWorldKey& Key, const uint8& Value);

};

UCLASS(Blueprintable)
class GOAPPROJECT_API UGOAPAction_BlueprintBase : public UGOAPAction
{
	GENERATED_BODY()

protected:
	bool bExecuteHasImpl;
	bool bTickHasImpl;
	bool bAbortHasImpl;

	bool bIsAborting;
public:

	UGOAPAction_BlueprintBase(const FObjectInitializer& ObjectInitializer);

	EActionResult StartAction() override;
	EActionResult AbortAction() override;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveExecuteAI(AAIController* AIController, APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveAbortAI(AAIController* AIController, APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveTickAI(AAIController* AIController, APawn* Pawn);

	UFUNCTION(BlueprintCallable)
		void FinishExecute(bool bSuccess);

	UFUNCTION(BlueprintCallable)
		void FinishAbort();

	void TickAction(float DeltaTime) override;
};

UCLASS()
class GOAPPROJECT_API UGOAPAction_RunEQSQuery : public UGOAPAction
{
	GENERATED_BODY()

protected:
	UPROPERTY()
		int32 RequestID;

	//Workaround for not being able to edit BBKeySelector outside of the BTEditor
	UPROPERTY(EditAnywhere)
		FName BBKeyName;

	UPROPERTY(EditAnywhere)
		struct FBlackboardKeySelector BlackboardKey;

	UPROPERTY(EditAnywhere)
		FEQSParametrizedQueryExecutionRequest EQSRequest;

	FQueryFinishedSignature QueryFinishedDelegate;
public:
	UGOAPAction_RunEQSQuery(const FObjectInitializer& Initializer);

	virtual EActionResult StartAction() override;
	virtual EActionResult AbortAction() override;

	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);
};

UCLASS()
class GOAPPROJECT_API UGOAPAction_Operator : public UGOAPAction, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Instanced)
		UAITask_Operator* Operator;

	UPROPERTY(transient)
		UAITask_Operator* OpInstance = nullptr;
public:
	
	UGOAPAction_Operator();

	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;


	EActionResult StartAction() override;
	void FinishAction(EPlannerTaskFinishedResult::Type Result) override;
	EActionResult AbortAction() override;

	virtual UAITask* GetOperator();
	void OnOperatorEnded();
	template<typename T>
	T* NewOperatorTask(AAIController& Controller)
	{
		return UAITask::NewAITask<T>(Controller, *this);
	}
};
typedef TMultiMap<EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;