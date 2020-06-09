#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPActionsComponent.generated.h"

class FGameplayDebuggerCategory;
class AAIController;
class UGOAPAction;
struct FStateNode;
struct FWorldState;

DECLARE_DELEGATE(FPlanCompletedSignature);

UCLASS()
class GOAPPROJECT_API UGOAPActionsComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(Transient)
		AAIController* AIOwner;
	
	UPROPERTY(Transient)
		UGOAPAction* CurrentAction;

	//it's fine if we don't store the solved plan context since it doesn't
	//really matter as long as the action preconditions are still valid for
	//the given plan step.

	//we can also revalidate the plan by iterating through the
	//remaining steps and applying the effects to a WS copy
	UPROPERTY(Transient)
		TArray<UGOAPAction*> ActionQueue;

	UPROPERTY()
		TArray<UGOAPAction*> ActionSet;

	int32 ActionIdx;

public:
	void OnRegister() override;

	UFUNCTION()
		void RegisterAction(TSubclassOf<UGOAPAction> ActionClass);

	UFUNCTION()
		void RegisterActionSet(const TArray<TSubclassOf<UGOAPAction>>& NewActionSet);
	//TODO:Add cleanup code to OnDestroy
	
	UFUNCTION()
		void OnActionEnded();

	UFUNCTION()
		void OnActionSuccess();

	UFUNCTION()
		void OnActionFailed();

	UFUNCTION()
	void RunNextAction();

	UFUNCTION()
		bool IsActionRunning();

	void StartPlan();

	UFUNCTION()
	void AbortPlan();

	//DON'T CALL THIS UNLESS YOU REALLY WANT YOUR ACTIONS TO BE GC'D
	UFUNCTION()
	void Reset();

	UFUNCTION()
	void QueueAction(UGOAPAction* Action);


	FPlanCompletedSignature OnPlanCompleted;

	UFUNCTION()
		TArray<UGOAPAction*>& GetActionSet();

protected:

	UFUNCTION()
		void ClearCurrentPlan();

public:


#if WITH_GAMEPLAY_DEBUGGER

	virtual void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};