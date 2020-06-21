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
		FString GoalName;

	UPROPERTY(config, EditAnywhere)
	TArray<FWorldProperty> Symbols;

	UPROPERTY(transient)
		AAIController* AIOwner;

	UPROPERTY(transient)
		UPlannerComponent* OwnerComp;
	//constant for now
	//TODO: use response curve
	UPROPERTY(EditAnywhere)
		float Insistence;
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
	const TArray<FWorldProperty>& GetSymbolSet();

	bool GetLastValidity() { return bCachedValidity; }
	void SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent);
	void InitGoal(AAIController* Controller);
	bool IsActive() { return bIsActive; }
	virtual bool IsGoalValid();
	virtual bool IsGoalStillValid();
	virtual void Activate();
	virtual void Deactivate();
	virtual void ReCalcPriority();

	float Priority() const;
};
