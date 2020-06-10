// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ActionTestingController.generated.h"

class UGOAPAction;
class UGOAPActionsComponent;
class UPlannerAsset;
/**
 * Meant to run an Action by keypress
 */
UCLASS(BlueprintType, Blueprintable)
class GOAPPROJECT_API AActionTestingController : public AAIController
{
	GENERATED_BODY()
	
protected:
	AActionTestingController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	//Want to allow this to be easy to edit without recompiling
	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<UGOAPAction*> TestActionArray;

	UPROPERTY()
		UGOAPActionsComponent* PlanComponent;
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void PlayAction();

	UFUNCTION(BlueprintCallable)
	void InterruptAction();

	UFUNCTION(BlueprintCallable)
		bool RunPlanner(UPlannerAsset* PlannerAsset);
};
