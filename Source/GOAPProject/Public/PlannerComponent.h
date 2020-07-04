#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "WorldState.h"
#include "WorldProperty.h"
#include "StateNode.h"
#include "PlannerComponent.generated.h"

class UGOAPAction;
class UGOAPGoal;
class UPlannerAsset;
class UPlannerService;
struct FStateNode;

struct GOAPPROJECT_API FAStarPlanner
{
	
private:
	typedef TSharedPtr<FStateNode> NodePtr;

	struct FPriorityQueue
	{
		TArray<NodePtr> Heap;
		TSharedPtrLess<FStateNode> LessFn;

		FPriorityQueue() : Heap(), LessFn()
		{
			Heap.Heapify(LessFn);
		}
		void Push(const NodePtr& InItem)
		{
			Heap.HeapPush(InItem, LessFn);
		}

		void Pop(NodePtr& OutItem)
		{
			Heap.HeapPop(OutItem, LessFn);
		}

		void ReSort()
		{
			Heap.HeapSort(LessFn);
		}

		TArray<NodePtr>::SizeType Num() const
		{
			return Heap.Num();
		}
	};

	TMultiMap<EWorldKey, TWeakObjectPtr<UGOAPAction>> EdgeTable;

public:
	int32 MaxDepth;
	
	bool Search(const TArray<FWorldProperty>& GoalCondition, const FWorldState& InitialState, TArray<UGOAPAction*>& Plan);
	void AddAction(UGOAPAction* Action);
	void RemoveAction(UGOAPAction* Action);
	void ClearEdgeTable();
};

UCLASS()
class GOAPPROJECT_API UPlannerComponent : public UBrainComponent
{
	GENERATED_BODY()
public:
	//unused for now. using messages first
	void OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result);
	void StartPlanner(UPlannerAsset& PlannerAsset);
	void RunAllActions();
	bool IsRunningPlan() const;
	void ScheduleReplan();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FString GetDebugInfoString() const;

	void SetWSProp(const EWorldKey& Key, const uint8& Value);

protected:

	FAStarPlanner AStarPlanner;

	bool bReplanNeeded = false;
	bool bPlanInProgress = false;
	bool bPlanUpdateNeeded = false;
	bool bWorldStateUpdated = false;

	UPROPERTY(transient)
		TArray<UGOAPAction*> ActionSet;

	UPROPERTY(transient)
		TArray<FAISymEffect> ExpectedEffects;

	UPROPERTY(transient)
		TArray<UPlannerService*> Services;

	UPROPERTY(transient)
		TArray<UGOAPGoal*> Goals;

	UPROPERTY(transient)
		UPlannerAsset* Asset;
	//this might need to get wrapped in an object so property-specific delegates can be added
	FWorldState WorldState;

	//After taking current action, used to validate effects if WS changes
	FWorldState PredictedWS;
	//Also add planner instance

	void ScheduleWSUpdate();

	void RequestExecutionUpdate();
	void UpdatePlanExecution();

	void ProcessReplanRequest();
	
	//Does not broadcast WS changes
	void SetWSPropInternal(const EWorldKey& Key, const uint8& Value);

	UPROPERTY(transient)
	TArray<UGOAPAction*> PlanBuffer;
	uint32 BufferSize = 1;
	uint32 PlanHead =0 ;
	uint32 PlanTail = 0;
	bool PlanFull = false;

	bool PlanReachedEnd();
	bool PlanAdvance();
	void StartNewPlan(TArray<UGOAPAction*>& Plan);
	void AddAction(UGOAPAction* Action);
	void ClearCurrentPlan();
};