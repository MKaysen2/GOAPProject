#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "WorldState.h"
#include "WorldProperty.h"
#include "StateNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "PlannerComponent.generated.h"

class UGOAPAction;
class UGOAPGoal;
class UPlannerAsset;
class UPlannerService;
struct FStateNode;


USTRUCT(BlueprintType)
struct GOAPPROJECT_API FPlanStepInfo
{
	GENERATED_BODY()

public:
	FPlanStepInfo() = default;
	FPlanStepInfo(const FPlanStepInfo& Other) = default;

		UPROPERTY()
		UGOAPAction* Action;

		UPROPERTY()
		FWorldState ResolvedWS;

		void SetAction(UGOAPAction* NewAction);
		void SetResolvedWS(const FWorldState& WS);


};

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
	
	bool Search(const TArray<FWorldProperty>& GoalCondition, const FWorldState& InitialState, TArray<FPlanStepInfo>& Plan);
	void AddAction(UGOAPAction* Action);
	void RemoveAction(UGOAPAction* Action);
	void ClearEdgeTable();
};

USTRUCT()
struct GOAPPROJECT_API FPlanInstance
{
	GENERATED_BODY()
public:
	int32 HeadIdx = 0;
	int32 TailIdx = 0;

	bool bInProgress = false;
	bool bFull = false;
	UPROPERTY()
		TArray<FPlanStepInfo> Buffer;


	void StartNewPlan(TArray<FPlanStepInfo>& Plan);
	void AddStep(const FPlanStepInfo& PlanStep);
	bool HasCurrentAction() const;
	UGOAPAction* GetCurrent();
	uint8 GetResolvedWSValue(EWorldKey Key);
	bool Advance();
	bool HasReachedEnd() const;
	void Init(int32 BufferSize);
	void Clear(bool bLeaveCurrent);
	bool IsRunningPlan() const;
};

UCLASS()
class GOAPPROJECT_API UPlannerComponent : public UBrainComponent
{
	GENERATED_BODY()
public:
	//unused for now. using messages first
	void OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result);
	void StartPlanner(UPlannerAsset& PlannerAsset);
	void StopPlanner();

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

	TMap<EWorldKey, uint8> ExpectedEffects;

	TMap<EWorldKey, uint8> GoalExpectedEffects;

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
	
	virtual void Cleanup() override;

	//Does not broadcast WS changes
	void SetWSPropInternal(const EWorldKey& Key, const uint8& Value);

	UPROPERTY(transient)
		UGOAPGoal* CurrentGoal;

	UFUNCTION(BlueprintCallable)
		uint8 GetResolvedValue(const EWorldKey& Key);

	UFUNCTION(BlueprintCallable)
		FName GetKeyName(uint8 KeyID);
	EActionStatus ActionStatus;

	UPROPERTY()
		FPlanInstance PlanInstance;

	void AbortPlan();

	void StartNewPlan(TArray<UGOAPAction*> Subtasks, TArray<FPlanStepInfo>& Plan);
};