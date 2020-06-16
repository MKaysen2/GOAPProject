// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\ActionTestingController.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\PlannerAsset.h"
#include "..\Public\PlannerComponent.h"
#include "..\Public\GOAPActionsComponent.h"

AActionTestingController::AActionTestingController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BrainComponent = CreateDefaultSubobject<UBrainComponent>(TEXT("BrainComp"));
}

void AActionTestingController::BeginPlay()
{
	Super::BeginPlay();
}

void AActionTestingController::PlayAction()
{
	UPlannerComponent* PlannerComp = Cast<UPlannerComponent>(BrainComponent);
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

	UPlannerComponent* PlannerComp = Cast<UPlannerComponent>(GetBrainComponent());
	if (PlannerComp == nullptr)
	{
		PlannerComp = NewObject<UPlannerComponent>(this, TEXT("PlannerComp"));
		PlannerComp->RegisterComponent();
		BrainComponent = PlannerComp;
	}
	PlannerComp->StartPlanner(*PlannerAsset);
	return true;
}