// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateNode.h"
#include "UObject/NoExportTypes.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "AStarComponent.generated.h"


#define ScreenLog(text)	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, text);

class UGOAPAction;
class UGOAPGoal;
class AAIController;

/** 
  AStarComponent
  Use to generate plans based on the current state and a given goal state and action set
 */
UCLASS(BlueprintType)
class GOAPPROJECT_API UAStarComponent : public UActorComponent
{

	GENERATED_BODY()
protected:
	typedef TMultiMap<EWorldKey, TWeakObjectPtr<UGOAPAction>> LookupTable;
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
	LookupTable ActionTable;

	UPROPERTY()
		AAIController* AIOwner;

	
	UPROPERTY()
		int32 MaxDepth;
public:
	UAStarComponent();
	void OnRegister() override;
	void OnUnregister() override;

	TSharedPtr<FStateNode> Search(UGOAPGoal* Goal, const FWorldState& InitialState);

	UFUNCTION()
	void AddAction(UGOAPAction* Action);

	//Not implemented yet
	UFUNCTION()
	void RemoveAction(UGOAPAction* Action);

	void ClearLookupTable();
	void CreateLookupTable(TArray<UGOAPAction*>& actions);
};
