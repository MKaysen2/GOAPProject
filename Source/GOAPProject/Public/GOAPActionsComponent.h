#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPActionsComponent.generated.h"


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
		TArray<UGOAPAction*> ActionsQueue;

	int32 ActionIdx;

	bool bPlanComplete;
public:
	void OnRegister() override;

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

	UFUNCTION()
		bool IsPlanComplete();

	FPlanCompletedSignature OnPlanCompleted;
};