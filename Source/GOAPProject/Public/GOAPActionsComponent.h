#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPActionsComponent.generated.h"


class AGOAPController;
class UGOAPAction;

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

	//These will be removed after implementing actions as event driven
	UFUNCTION()
		void ActionEnded();
	UFUNCTION()
		void OnMontageEnded();
};