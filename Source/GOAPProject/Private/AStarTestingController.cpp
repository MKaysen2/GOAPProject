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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .2f;
}

void AAStarTestingController::BeginPlay()
{
}

void AAStarTestingController::Tick(float DeltaSeconds)
{
	if (IsValid(DummyGoal))
	{
		AStarComponent->Search(DummyGoal, CurrentState);
	}
}

void AAStarTestingController::OnPossess(APawn* InPawn)
{
	DummyGoal = NewObject<UAIGoal_AlwaysValid>(this);
	DummyGoal->InitGoal(this);
	UGOAPAction* NewAction = NewObject<UAIAct_Attack>(this);
	NewAction->InitAction(this);
	Action.Add(NewAction);
	AStarComponent->CreateLookupTable(Action);
}

void AAStarTestingController::OnUnPossess()
{
	DummyGoal = nullptr;
	Action.Reset();
}
