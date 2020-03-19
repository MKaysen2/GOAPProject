#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPActionsComponent.generated.h"

class FGameplayDebuggerCategory;
class AGOAPController;
class UGOAPAction;

DECLARE_DELEGATE(FPlanCompletedSignature);

UCLASS()
class GOAPPROJECT_API UGOAPActionsComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(Transient)
		AGOAPController* AIOwner;
	
	//This should be changed to WeakPtr to avoid increasing ref count
	UPROPERTY(Transient)
		UGOAPAction* CurrentAction;

	UPROPERTY(Transient)
		TArray<UGOAPAction*> ActionQueue;

	UPROPERTY(Transient)
		TArray<UGOAPAction*> ActionSet;

	int32 ActionIdx;

	bool bPlanComplete;
public:
	void OnRegister() override;

	UFUNCTION()
		void RegisterAction(TSubclassOf<UGOAPAction> ActionClass);

	UFUNCTION()
		void RegisterActionSet(const TArray<TSubclassOf<UGOAPAction>>& NewActionSet);
	//TODO:Add cleanup code to OnDestroy
	UFUNCTION()
	void RunNextAction();

	UFUNCTION()
		bool IsActionRunning();
	UFUNCTION()
	void AbortPlan();

	UFUNCTION()
	void Reset();

	UFUNCTION()
	void QueueAction(UGOAPAction* Action);

	FPlanCompletedSignature OnPlanCompleted;

	UFUNCTION()
		bool IsPlanComplete();
	UFUNCTION()
		TArray<UGOAPAction*>& GetActionSet();


#if WITH_GAMEPLAY_DEBUGGER
	virtual void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};