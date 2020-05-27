// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\AStarTestingController.h"
#include "..\Public\GOAPGoal.h"
#include "..\Public\AStarComponent.h"
#include "..\Public\WorldProperty.h"
#include "..\Public\AIAct_Attack.h"

AAStarTestingController::AAStarTestingController()
	: Super()
{
	AStarComponent = CreateDefaultSubobject<UAStarComponent>(FName("AStarComponent"));

	CurrentState = MakeShared<FWorldState>();
	DummyGoal = nullptr;
}

void AAStarTestingController::Plan()
{
	if (GetPawn() && IsValid(DummyGoal))
	{
		AStarComponent->Search(DummyGoal, CurrentState);
	}
}

void AAStarTestingController::BeginPlay()
{
	Super::BeginPlay();
}

void AAStarTestingController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	DummyGoal = NewObject<UGOAPGoal>(this, GoalClass);
	DummyGoal->InitGoal(this);
	for (auto& ActionClass : ActionClasses)
	{
		UGOAPAction* NewAction = NewObject<UGOAPAction>(this, ActionClass);
		NewAction->InitAction(this);
		Actions.Add(NewAction);
	}
	AStarComponent->CreateLookupTable(Actions);
}

void AAStarTestingController::OnUnPossess()
{
	DummyGoal = nullptr;
	Actions.Reset();
}
