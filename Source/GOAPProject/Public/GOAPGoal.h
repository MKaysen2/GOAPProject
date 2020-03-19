#pragma once
#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "EngineDefines.h"
#include "GOAPGoal.generated.h"

class AGOAPCharacterBase;
class AAIController;

DECLARE_LOG_CATEGORY_EXTERN(LogGoal, Warning, All);

UCLASS(ABSTRACT, BlueprintType)
class UGOAPGoal : public UObject 
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FWorldProperty> Goal;

	UPROPERTY()
	float LastPriority;

	UPROPERTY()
		bool bIsActive;

	UPROPERTY()
		bool bCachedValidity;

	UFUNCTION()
		void CacheValidity(bool Validity) { bCachedValidity = Validity; }
public:
	UGOAPGoal();
	const TArray<FWorldProperty>& container();

	bool GetLastValidity() { return bCachedValidity; }

	bool IsActive() { return bIsActive; }
	virtual bool IsGoalValid(AAIController* Controller);
	virtual void Activate(AAIController* Controller);
	virtual void Deactivate(AAIController* Controller);
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

UCLASS()
class UAIGoal_InvestigateNoise : public UGOAPGoal
{
	GENERATED_BODY()
public:
	UAIGoal_InvestigateNoise();
	bool IsGoalValid(AAIController* Controller) override;
	void Activate(AAIController* Controller) override;
};