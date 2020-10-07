// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\ActionTestingController.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\PlannerAsset.h"
#include "..\Public\PlannerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AActionTestingController::AActionTestingController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlannerComp = CreateDefaultSubobject<UPlannerComponent>(TEXT("PlannerComp"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	SetGenericTeamId(FGenericTeamId(2));
}

void AActionTestingController::BeginPlay()
{
	Super::BeginPlay();
}

void AActionTestingController::PlayAction()
{
	if (PlannerComp && !PlannerComp->IsRunningPlan())
	{
		PlannerComp->RunAllActions();
	}
}

void AActionTestingController::InterruptAction()
{
}

bool AActionTestingController::RunPlanner(UPlannerAsset* PlannerAsset)
{
	if (PlannerAsset == nullptr)
	{
		return false;
	}

	if (PlannerComp == nullptr)
	{
		PlannerComp = NewObject<UPlannerComponent>(this, TEXT("PlannerComp"));
		PlannerComp->RegisterComponent();
	}
	PlannerComp->StartPlanner(*PlannerAsset);
	return true;
}