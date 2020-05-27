#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.h"
#include "Templates/UniquePtr.h"
#include "Templates/SharedPointer.h"
#include "StateNode.generated.h"

class UGOAPAction;
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
	typedef TMultiMap < EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;
	//Can make sharedref or uniqueptr to show ownership
		TSharedPtr<FWorldState> CurrentState;
		
		//TODO: Should be weakptr
		TSharedPtr<FWorldState> GoalState; //Not a new object

		//TODO: Should be a weakptr to prevent reference cycles
		TSharedPtr<FStateNode> ParentNode;

	UPROPERTY()
		UGOAPAction* ParentEdge;

		int CountUnsatisfied();
	int ForwardCost;
	int unsatisfied;
	int Depth = 0;

	bool Closed;
public:
	

	friend FORCEINLINE bool operator<(const FStateNode& lhs, const FStateNode& rhs) {
		return lhs.cost() < rhs.cost();
	}
	FStateNode();
	FStateNode(const TArray<FWorldProperty>& GoalSet, TSharedPtr<FWorldState> InitialState);
	FStateNode(TSharedPtr<FStateNode> Node, UGOAPAction* Edge);

	int cost() const;
	int GetDepth() const;
	void MarkClosed();
	void MarkOpened();
	bool IsClosed();

	int GetForwardCost();
	/** Reparent the node to use another Node's parent node and Edge
	  * Also update the forward cost to match
	  */
	void ReParent(const FStateNode& OtherNode);
	void TakeAction(const UGOAPAction* action);
	void UnapplyProperty(const FWorldProperty& Property);
	void AddPrecondition(const FWorldProperty& Property);

	bool IsGoal();
	void FindActions(const LookupTable& action_map, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions);
	void LogNode() const;

	void LogGoal() const 
	{
		UE_LOG(LogWS, Warning, TEXT("(Goal)"));
		GoalState->LogWS();
	}

	uint32 GetWSTypeHash()
	{
		return CurrentState->GetArrayTypeHash();
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

	struct SetKeyFuncs : public BaseKeyFuncs<TSharedPtr<FStateNode>, TSharedPtr<FStateNode>>
	{
		typedef typename TCallTraits<TSharedPtr<FStateNode>>::ParamType KeyInitType;
		typedef typename TCallTraits<TSharedPtr<FStateNode>>::ParamType ElementInitType;

		/**
		 * @return The key used to index the given element.
		 */
		static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
		{
			return Element;
		}

		/**
		 * @return True if the keys match.
		 */
		template<typename ComparableKey>
		static FORCEINLINE bool Matches(KeyInitType A, ComparableKey B)
		{
			return (A.IsValid() && B.IsValid()) && (A->GetWSTypeHash() == B->GetWSTypeHash());
		}

		/** Calculates a hash index for a key. */
		static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
		{
			return Key.IsValid() ? Key->GetWSTypeHash() : GetTypeHash(NULL);
		}
	};
};