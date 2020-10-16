#pragma once
#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "BehaviorTree/BehaviorTreeTypes.h"

#include "GOAPGoal.generated.h"

class UGOAPAction;
struct FWorldState;
class AAIController;
class UPlannerComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogGoal, Warning, All);


//TODO: add Decorators
UCLASS(Config=AI, EditInlineNew, BlueprintType)
class UGOAPGoal : public UObject 
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		FString TaskName;

	//could also add an "ALL"/"ANY" satisfaction specifier
	//Since "ANY" would just mean making a start node
	//for each condition when you do a search
	UPROPERTY(config, EditAnywhere)
	TArray<FWorldProperty> GoalCondition;

	//Subtasks that need to happen before the found plan happens
	UPROPERTY(EditAnywhere, Instanced)
		TArray<UGOAPAction*> SubTasks;

	//TODO: Everything below this should go in a separate interface
	//WS conditions for whether the goal is valid or not
	UPROPERTY(config, EditAnywhere)
		TArray<FWorldProperty> Preconditions;

	UPROPERTY(transient)
		AAIController* AIOwner;

	UPROPERTY(transient)
		UPlannerComponent* OwnerComp;
	//constant for now
	//TODO: use response curve
	UPROPERTY(EditAnywhere)
		float Insistence = 1.0;

	UPROPERTY()
		bool bCachedValidity;

	UFUNCTION()
		void CacheValidity(bool Validity) { bCachedValidity = Validity; }
public:
	UGOAPGoal();
	const TArray<FWorldProperty>& GetGoalCondition() const { return GoalCondition;  }
	TArray<UGOAPAction*> GetSubTasks() const { return SubTasks; }
	FString GetTaskName() { return TaskName;  }
	bool IsValid() const { return bCachedValidity; }
	void SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent);
	void OnWSUpdated(const FWorldState& WorldState);
	float GetInsistence() const;
};
