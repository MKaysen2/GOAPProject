#pragma once
#include "CoreMinimal.h"
#include "WorldProperty.h"
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
		AAIController* AIOwner;

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

	void InitGoal(AAIController* Controller);
	bool IsActive() { return bIsActive; }
	virtual bool IsGoalValid();
	virtual bool IsGoalStillValid();
	virtual void Activate();
	virtual void Deactivate();
	virtual void ReCalcPriority();

	float Priority() const;
};

UCLASS()
class UAIGoal_KillEnemy : public UGOAPGoal
{
	GENERATED_BODY()

		UPROPERTY()
		AActor* CachedTarget;
public:
	UAIGoal_KillEnemy();
	bool IsGoalValid() override;
	void Activate() override;
	void ReCalcPriority() override;
};

UCLASS()
class UAIGoal_Death : public UGOAPGoal
{
	GENERATED_BODY()
public:
	UAIGoal_Death();
	bool IsGoalValid() override;
	void Activate() override;
};

UCLASS()
class UAIGoal_AlwaysValid : public UGOAPGoal
{
	GENERATED_BODY()
public:
	UAIGoal_AlwaysValid();
	bool IsGoalValid() override;
};