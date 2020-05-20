// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "IAStarNode.h"
#include "StateNode.h"
#include "Containers/Map.h"
#include "UObject/NoExportTypes.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "AStarComponent.generated.h"


#define ScreenLog(text)	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, text);

class UGOAPAction;
class UGOAPGoal;
class UWorldState;
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
	typedef TMultiMap<EWorldKey, UGOAPAction*> LookupTable;
	LookupTable ActionTable;

	UPROPERTY()
		AAIController* AIOwner;

	UPROPERTY()
		int32 MaxDepth;
public:
	UAStarComponent();
	void OnRegister() override;
	void OnUnregister() override;

	TSharedPtr<FStateNode> Search(UGOAPGoal* Goal, TSharedPtr<FWorldState>& InitialState);

	void ClearLookupTable();
	void CreateLookupTable(TArray<UGOAPAction*>& actions);
};
