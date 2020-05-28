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
		TSharedRef<FWorldState> CurrentState;
		
		//The true current state, that we are regressing to
		//Can probably be a sharedref too since it's not supposed to be null, ever
		TSharedPtr<FWorldState> GoalState;

		TWeakPtr<FStateNode> ParentNode;

	UPROPERTY()
		UGOAPAction* ParentEdge;

	/**Properties flags (just Unsatisfied for now). Indexed by EWorldKey
	  * might make this into uint8 and add some flags if I need to */
	UPROPERTY()
		TArray<bool> PropFlags;

	int ForwardCost;
	int NumUnsatisfied;
	int Depth = 0;
		
	bool Closed;

	int CountUnsatisfied();

public:
	
	FStateNode();
	FStateNode(const TArray<FWorldProperty>& GoalSet, TSharedPtr<FWorldState> InitialState);
	FStateNode(const TSharedRef<FWorldState>& WorldState, const TSharedPtr<FStateNode>& Node);

	static TSharedPtr<FStateNode> CreateStartNode(const TArray<FWorldProperty>& GoalSet, const TSharedPtr<FWorldState>& InitialState);
	static TSharedPtr<FStateNode> GenerateNeighbor(const TSharedPtr<FStateNode>& CurrentNode, UGOAPAction* Action);

	friend FORCEINLINE bool operator<(const FStateNode& lhs, const FStateNode& rhs) {
		return lhs.Cost() < rhs.Cost();
	}

	int Cost() const;
	int GetForwardCost();
	int GetDepth() const;
	void MarkClosed();
	void MarkOpened();
	bool IsClosed();
	void SetUnsatisfied(EWorldKey Key);

	/** Reparent the node to use another Node's parent node and Edge
	  * Also update the forward cost to match
	  */
	void ReParent(const FStateNode& OtherNode);
	bool ChainBackward(UGOAPAction* action);
	void UnapplyProperty(const FWorldProperty& Property);
	void AddPrecondition(const FWorldProperty& Property);

	bool IsGoal();
	void GetNeighboringEdges(const LookupTable& action_map, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions);

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

	UGOAPAction* edge() 
	{
		return ParentEdge;
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