#pragma once
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "GoalSelectionComponent.generated.h"

class UGOAPGoal;
class FGameplayDebuggerCategory;
class UBrainComponent;
DECLARE_DELEGATE_OneParam(FGoalChangedSignature, UGOAPGoal*);
//TODO: Merge with BrainComponent and have goals add listeners for AIMessages

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
	
	void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		UGOAPGoal* GetCurrentGoal();

	UFUNCTION()
		void ReEvaluateGoals();

	//It would be cool to dynamically update available goals
	UFUNCTION()
		void RegisterGoal(TSubclassOf<UGOAPGoal> GoalClass);

	UFUNCTION()
		void OnGoalCompleted();

	UFUNCTION()
		void RegisterGoalSet(const TArray<TSubclassOf<UGOAPGoal>>& NewGoalSet);

	UFUNCTION()
		virtual void Reset();

	FGoalChangedSignature OnGoalChanged;
	//Debug
#if WITH_GAMEPLAY_DEBUGGER
	virtual void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER
};
