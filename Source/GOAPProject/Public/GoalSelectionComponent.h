#pragma once
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "GoalSelectionComponent.generated.h"

class UGOAPGoal;
class FGameplayDebuggerCategory;

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
		TArray<UGOAPGoal*> GoalSet;
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

	//It would be cool to dynamically update available goals
	UFUNCTION()
		void RegisterGoal(TSubclassOf<UGOAPGoal> GoalClass);

	UFUNCTION()
		void OnGoalCompleted();

#if WITH_GAMEPLAY_DEBUGGER
	virtual void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

	UFUNCTION()
		void RegisterGoalSet(const TArray<TSubclassOf<UGOAPGoal>>& NewGoalSet);
};
