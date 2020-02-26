#pragma once
#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "GOAPGoal.generated.h"

class AGOAPCharacterBase;
class AAIController;

UCLASS(ABSTRACT, BlueprintType)
class UGOAPGoal : public UObject 
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FWorldProperty> goal;
	float LastPriority;
public:
	UGOAPGoal();
	const TArray<FWorldProperty>& container();
	virtual bool IsGoalValid(AAIController* Controller);
	virtual void Activate(AAIController* Controller);
	virtual void ReCalcPriority(AAIController* Controller);
	float Priority() const;
};

UCLASS()
class UAIGoal_KillEnemy : public UGOAPGoal
{
	GENERATED_BODY()
public:
	UAIGoal_KillEnemy();
	bool IsGoalValid(AAIController* Controller) override;
	void Activate(AAIController* Controller) override;
	void ReCalcPriority(AAIController* Controller) override;
};

UCLASS()
class UAIGoal_Wander : public UGOAPGoal
{
	GENERATED_BODY()
public:
	UAIGoal_Wander();
	bool IsGoalValid(AAIController* Controller) override;
	void Activate(AAIController* Controller) override;
	void ReCalcPriority(AAIController* Controller) override;
};