#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "GOAPAction.h"
#include "IAStarNode.h"

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
	UPROPERTY(EditAnywhere)
		UWorldState* CurrentState;

	UPROPERTY(VisibleAnywhere)
		const UWorldState* GoalState; //Not a new object

		TSharedPtr<FStateNode> ParentNode;
	UPROPERTY()
		UGOAPAction* ParentEdge;

		int CountUnsatisfied();
	int forward_cost;

public:
	int unsatisfied;
	int depth = 0;

	friend FORCEINLINE bool operator<(const FStateNode& lhs, const FStateNode& rhs) {
		return lhs.cost() < rhs.cost();
	}
	FStateNode();
	FStateNode(TSharedPtr<FStateNode> Node, UGOAPAction* Edge);
	void SetupInitialNode(const TArray<FWorldProperty>& goal, const UWorldState* CurrentState); //regressive search
	int cost() const;
	void TakeAction(const UGOAPAction* action);


	bool IsGoal();
	void FindActions(const LookupTable& action_map, TArray<UGOAPAction*>& out_actions);

	UGOAPAction* edge() {
		return ParentEdge;
	}
	TSharedPtr<FStateNode> previous() {
		return ParentNode;
	}
};