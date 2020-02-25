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
class AGOAPController;
/**
 * This is really like a GOAPComponent
 */
UCLASS(BlueprintType)
class GOAPPROJECT_API UAStarComponent : public UActorComponent
{
	
	GENERATED_BODY()
protected:
	typedef TMultiMap<FWorldProperty, UGOAPAction*> LookupTable;
	LookupTable ActionTable;
	void generate_plan(TSharedPtr<FStateNode> FoundGoal, TArray<UGOAPAction*>& out_plan);
	UPROPERTY()
		AGOAPController* AIOwner;
public:
	UAStarComponent();
	void OnRegister() override;
	void OnUnregister() override;
	bool Search(UGOAPGoal* Goal, const UWorldState* CurrentState, TArray<UGOAPAction*>& Plan);

	void ClearLookupTable();
	void CreateLookupTable(TArray<UGOAPAction*>& actions);
};
