#pragma once
#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "GOAPGoal.generated.h"

class AAIController;
class UPlannerComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogGoal, Warning, All);

UCLASS(Config=AI, EditInlineNew, BlueprintType)
class UGOAPGoal : public UObject 
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		FString TaskName;

	UPROPERTY(config, EditAnywhere)
	TArray<FWorldProperty> GoalCondition;

	UPROPERTY(transient)
		AAIController* AIOwner;

	UPROPERTY(transient)
		UPlannerComponent* OwnerComp;
	//constant for now
	//TODO: use response curve
	UPROPERTY(EditAnywhere)
		float Insistence;

	UPROPERTY()
		bool bCachedValidity;

	UFUNCTION()
		void CacheValidity(bool Validity) { bCachedValidity = Validity; }
public:
	UGOAPGoal();
	const TArray<FWorldProperty>& GetGoalCondition() const { return GoalCondition;  }

	FString GetTaskName() { return TaskName;  }
	bool IsValid() const { return bCachedValidity; }
	void SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent);

	float GetInsistence() const;
};
