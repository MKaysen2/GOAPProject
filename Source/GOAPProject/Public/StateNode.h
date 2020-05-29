#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.h"
#include "Templates/SharedPointer.h"

class UGOAPAction;
template <typename T>
struct GOAPPROJECT_API TSharedPtrLess 
{

	FORCEINLINE bool operator()(const TSharedPtr<T>& A, const TSharedPtr<T>& B) const 
	{
		return *A < *B;
	}
};

struct GOAPPROJECT_API FStateNode : public TSharedFromThis<FStateNode>
{
private:
	typedef TMultiMap < EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;
	//Can make sharedref or uniqueptr to show ownership
		TSharedRef<FWorldState> CurrentState;
		
		//The true current state, that we are regressing to
		//Can probably be a sharedref too since it's not supposed to be null, ever
		TSharedRef<FWorldState> GoalState;

		TWeakPtr<const FStateNode> ParentNode;

		TWeakObjectPtr<UGOAPAction> ParentEdge;

		TSet<EWorldKey> UnsatisfiedKeys;
	/**Properties flags (just Unsatisfied for now). Indexed by EWorldKey
	  * might make this into uint8 and add some flags if I need to */

		TArray<bool> PropFlags;

	int ForwardCost;
	int Heuristic;
	int TotalCost;
	bool Closed;
	int Depth = 0;
	void CacheTotalCost();
	bool IsSatisfied(EWorldKey Key) const;

	//Whether a value of the world state must hold to satisfy some precondition
	void AddPrecondition(const EWorldKey& Key, const uint8& Value);

public:
	
	FStateNode() = delete;
	FStateNode(const FWorldState& InitialState, const TArray<FWorldProperty>& SymbolSet);
	FStateNode(const FStateNode& Node);

	friend FORCEINLINE bool operator<(const FStateNode& lhs, const FStateNode& rhs) {
		return lhs.GetCost() < rhs.GetCost();
	}

	int GetCost() const;
	int GetForwardCost() const;
	int GetDepth() const;
	void MarkClosed();
	void MarkOpened();
	bool IsClosed() const;
	void SetUnsatisfied(EWorldKey Key);

	/** Reparent the node to use another Node's parent node and Edge
	  * Also update the forward cost to match
	  */
	void ReParent(const FStateNode& OtherNode);
	bool ChainBackward(UGOAPAction& Action);

	//Applies the inverse of the effect to the WS value for Key
	//The inverse of the "set" effect is to revert the value to whatever it was in the goal state
	//returns false if the heuristic increased in value
	bool InvertEffect(const EWorldKey& Key);

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

	TWeakObjectPtr<UGOAPAction> edge() 
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