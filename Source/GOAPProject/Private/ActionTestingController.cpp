// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\ActionTestingController.h"
#include "..\Public\GOAPAction.h"
#include "Engine/World.h"

void AActionTestingController::BeginPlay()
{
	Super::BeginPlay();
	if (*ActionClass)
	{
		Action = NewObject<UGOAPAction>(this, ActionClass);
		Action->InitAction(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create action"));

	}
}

void AActionTestingController::PlayAction()
{
	UE_LOG(LogTemp, Warning, TEXT("Play action"));
	if (!Action)
	{
		UE_LOG(LogTemp, Error, TEXT("Action was null!!"));
		return;
	}
	if (Action->IsActionRunning())
	{
		UE_LOG(LogTemp, Warning, TEXT("the action was running already"));
		return;
	}
	if (!Action->VerifyContext())
	{
		UE_LOG(LogTemp, Error, TEXT("Verify context failed!!"));

		return;
	}
	if (Action->StartAction() == EActionStatus::kFailed)
	{
		UE_LOG(LogTemp, Error, TEXT("the action failed to run properly"));
	}
}

void AActionTestingController::InterruptAction()
{
	if (Action && Action->IsActionRunning())
	{
		Action->AbortAction();
	}
}
