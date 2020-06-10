// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\ActionTestingController.h"
#include "..\Public\GOAPAction.h"
#include "Engine/World.h"
#include "..\Public\GOAPActionsComponent.h"

void AActionTestingController::BeginPlay()
{
	Super::BeginPlay();
	PlanComponent = CreateDefaultSubobject<UGOAPActionsComponent>(TEXT("PlanComp"));
	BrainComponent = CreateDefaultSubobject<UBrainComponent>(TEXT("BrainComp"));

}

void AActionTestingController::PlayAction()
{
	PlanComponent->StartPlan(TestActionArray);
}

void AActionTestingController::InterruptAction()
{
	PlanComponent->AbortPlan();
}
