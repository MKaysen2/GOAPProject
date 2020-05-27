// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WorldState.h"
#include "AStarTestingController.generated.h"

class UAStarComponent;
class UGOAPAction;
class UGOAPGoal;
/**
 * Basically just replans every think cycle without ever executing any actions
 */
UCLASS()
class GOAPPROJECT_API AAStarTestingController : public AAIController
{
	GENERATED_BODY()
	
protected:

	TSharedPtr<FWorldState> CurrentState;
	//The main offender
	UPROPERTY()
		UAStarComponent* AStarComponent;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UGOAPGoal> GoalClass;
	//always valid, I think
	UPROPERTY(transient)
		UGOAPGoal* DummyGoal;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UGOAPAction>> ActionClasses;
	//The only action
	UPROPERTY(transient)
		TArray<UGOAPAction*> Actions;

public:

	AAStarTestingController();
	UFUNCTION(BlueprintCallable)
		void Plan();
	UFUNCTION()
		void BeginPlay() override;

	UFUNCTION()
		void OnPossess(APawn* InPawn) override;
	UFUNCTION()
		void OnUnPossess() override;
};
