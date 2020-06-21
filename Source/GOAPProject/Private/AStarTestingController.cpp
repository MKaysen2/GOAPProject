// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\AStarTestingController.h"
#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\AStarComponent.h"
#include "..\Public\WorldProperty.h"

AAStarTestingController::AAStarTestingController()
	: Super()
{
	AStarComponent = CreateDefaultSubobject<UAStarComponent>(FName("AStarComponent"));

	CurrentState = MakeShared<FWorldState>();
	DummyGoal = nullptr;
}

void AAStarTestingController::Plan()
{
}

void AAStarTestingController::BeginPlay()
{
	Super::BeginPlay();
}

void AAStarTestingController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAStarTestingController::OnUnPossess()
{
}
