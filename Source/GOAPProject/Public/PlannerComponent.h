#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "WorldState.h"
#include "WorldProperty.h"
#include "PlannerComponent.generated.h"

class UGOAPAction;
class UPlannerAsset;
UCLASS()
class GOAPPROJECT_API UPlannerComponent : public UBrainComponent
{
	GENERATED_BODY()
public:
	//unused for now. using messages first
	void OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result);
	void StartPlanner(UPlannerAsset& PlannerAsset);
	void RunAllActions();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FString GetDebugInfoString() const;
protected:

	bool bPlanUpdateNeeded = false;
	UPROPERTY(transient)
		TArray<UGOAPAction*> ActionSet;

	UPROPERTY(transient)
		UPlannerAsset* Asset;
	//this might need to get wrapped in an object so property-specific delegates can be added
	FWorldState WorldState;
	//Also add planner instance

	void RequestExecutionUpdate();
	void UpdatePlanExecution();

	UPROPERTY(transient)
	TArray<UGOAPAction*> PlanBuffer;
	uint32 BufferSize = 1;
	uint32 PlanHead;
	uint32 PlanTail;
	bool PlanFull = false;

	bool PlanAdvance();
	void StartNewPlan(TArray<UGOAPAction*>& Plan);
	void AddAction(UGOAPAction* Action);
	void ClearCurrentPlan();
};