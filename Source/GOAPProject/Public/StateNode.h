#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "GOAPAction.h"
#include "IAStarNode.h"
#include "Templates/UniquePtr.h"
#include "Templates/SharedPointer.h"
#include "StateNode.generated.h"

template <typename T>
struct GOAPPROJECT_API TSharedPtrLess 
{

	FORCEINLINE bool operator()(const TSharedPtr<T>& A, const TSharedPtr<T>& B) const 
	{
		return *A < *B;
	}
};

USTRUCT(BlueprintType)
struct GOAPPROJECT_API FStateNode
{
	GENERATED_BODY()
private:
		TSharedPtr<FWorldState> CurrentState;
	
		TSharedPtr<FWorldState> GoalState; //Not a new object

		TSharedPtr<FStateNode> ParentNode;

	UPROPERTY()
		UGOAPAction* ParentEdge;

		int CountUnsatisfied();
	int forward_cost;

public:
	int unsatisfied;
	int Depth = 0;

	friend FORCEINLINE bool operator<(const FStateNode& lhs, const FStateNode& rhs) {
		return lhs.cost() < rhs.cost();
	}
	FStateNode();
	FStateNode(const TArray<FWorldProperty>& GoalSet, TSharedPtr<FWorldState> InitialState);
	FStateNode(TSharedPtr<FStateNode> Node, UGOAPAction* Edge);

	int cost() const;
	void TakeAction(const UGOAPAction* action);
	void UnapplyProperty(const FWorldProperty& Property);
	void AddPrecondition(const FWorldProperty& Property);

	bool IsGoal();
	void FindActions(const LookupTable& action_map, TArray<UGOAPAction*>& out_actions);
	void LogNode() const;

	void LogGoal() const 
	{
		UE_LOG(LogWS, Warning, TEXT("(Goal)"));
		GoalState->LogWS();
	}
	TSharedPtr<FWorldState> GetState()
	{
		return CurrentState;
	}
	UGOAPAction* edge() 
	{
		return ParentEdge;
	}
	TSharedPtr<FStateNode> previous() 
	{
		return ParentNode;
	}
};