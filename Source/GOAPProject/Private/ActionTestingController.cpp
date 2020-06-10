// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\ActionTestingController.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\PlannerAsset.h"
#include "..\Public\GOAPActionsComponent.h"

AActionTestingController::AActionTestingController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlanComponent = CreateDefaultSubobject<UGOAPActionsComponent>(TEXT("PlanComp"));
	BrainComponent = CreateDefaultSubobject<UBrainComponent>(TEXT("BrainComp"));
}
void AActionTestingController::BeginPlay()
{
	Super::BeginPlay();
	for (auto* Action : TestActionArray)
	{
		Action->InitAction(this);
	}
}

void AActionTestingController::PlayAction()
{
	PlanComponent->StartPlan(TestActionArray);
}

void AActionTestingController::InterruptAction()
{
	PlanComponent->AbortPlan();
}

bool AActionTestingController::RunPlanner(UPlannerAsset* PlannerAsset)
{
	return false;
}