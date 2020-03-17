#pragma once
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "GoalSelectionComponent.generated.h"

class UGOAPGoal;

//TODO: Add a delegate for goal changed

UCLASS(BlueprintType)
class GOAPPROJECT_API UGoalSelectionComponent : public UActorComponent 
{
	GENERATED_BODY()
protected:
	UPROPERTY(Transient)
		AAIController* AIOwner;

	UPROPERTY(Transient)
		UGOAPGoal* CurrentGoal;
	UPROPERTY(Transient)
		UGOAPGoal* NextGoal;

	UPROPERTY(Transient)
		TArray<UGOAPGoal*> Goals;
public:
	UGoalSelectionComponent();
	UFUNCTION()
		virtual void OnRegister() override;

	UFUNCTION()
		UGOAPGoal* GetCurrentGoal();

	//TODO: move this into a separate componennt
	UFUNCTION()
		void ReEvaluateGoals();
	UFUNCTION()
		bool HasGoalChanged();

	UFUNCTION()
		void RegisterGoal(TSubclassOf<UGOAPGoal> GoalClass);
};
