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
public:
	typedef TMultiMap < EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;
	//Can make sharedref or uniqueptr to show ownership
	//TODO: this doesn't need to be a smart pointer at all
	TSharedRef<FWorldState> CurrentState;

	//The true current state, that we are regressing to
	//Can probably be a sharedref too since it's not supposed to be null, ever
	TSharedRef<FWorldState> GoalState;

	TWeakPtr<const FStateNode> ParentNode;

	TWeakObjectPtr<UGOAPAction> ParentEdge;

	TSet<EWorldKey> UnsatisfiedKeys;

	TMap<EWorldKey, FWorldProperty> UnsatisfiedPreconditions;
	/**Properties flags (just Relevant for now). Indexed by EWorldKey
	  * might make this into uint8 and add some flags if I need to */


	TArray<bool> PropFlags;

	int ForwardCost;
	int Heuristic;
	int TotalCost;
	bool Closed;
	int Depth = 0;
	uint32 CachedHash;

	void CacheTotalCost();

	//Whether a value of the world state must hold to satisfy some precondition
	bool AddPrecondition(const FWorldProperty& Precondition);

	bool GetKeyRelevance(const EWorldKey& Key)
	{
		return PropFlags[(uint8)Key];
	}

	void SetKeyRelevance(const EWorldKey& Key, bool bRelevance)
	{
		PropFlags[(uint8)Key] = bRelevance;
	}

	void CacheTypeHash(uint32 Hash)
	{
		CachedHash = Hash;
	}
public:
	
	FStateNode() = delete;
	FStateNode(const FWorldState& InitialState, const TArray<FWorldProperty>& SymbolSet);
	FStateNode(const FStateNode& Node);

	friend FORCEINLINE bool operator<(const FStateNode& lhs, const FStateNode& rhs) {
		return lhs.GetCost() < rhs.GetCost();
	}

	friend FORCEINLINE uint32 GetTypeHash(const FStateNode& Node)
	{
		return Node.CachedHash;
	}

	static constexpr bool Index(const EWorldKey& Key)
	{
		return (uint8)Key;
	}
	int GetCost() const;
	int GetForwardCost() const;
	int GetDepth() const;
	void MarkClosed();
	void MarkOpened();
	bool IsClosed() const;

	/** Reparent the node to use another Node's parent node and Edge
	  * Also update the forward cost to match
	  */
	void ReParent(const FStateNode& OtherNode);
	bool ChainBackward(UGOAPAction& Action);

	//Applies the inverse of the effect to the WS value for Key
	//The inverse of the "set" effect is to revert the value to whatever it was in the goal state
	//returns false if the heuristic increased in value
	bool InvertEffect(const EWorldKey& Key, const FAISymEffect& Effect);

	bool IsGoal();
	void GetNeighboringEdges(const LookupTable& action_map, TArray<TWeakObjectPtr<UGOAPAction>>& out_actions);

	uint32 GetWSTypeHash() const
	{
		return GetTypeHash(CurrentState.Get());
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
			return (A.IsValid() && B.IsValid()) && (GetTypeHash(*A) == GetTypeHash(*B));
		}

		/** Calculates a hash index for a key. */
		static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
		{
			return Key.IsValid() ? GetTypeHash(*Key) : GetTypeHash(NULL);
		}
	};

	void LogNode() const;

	void LogGoal() const
	{
		UE_LOG(LogWS, Warning, TEXT("(Goal)"));
		GoalState->LogWS();
	}
};