// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\ActionTestingController.h"
#include "..\Public\GOAPAction.h"
#include "Engine/World.h"

void AActionTestingController::BeginPlay()
{
	Super::BeginPlay();
	
	Action = NewObject<UGOAPAction>(this, ActionClass);
	Action->InitAction(this);
}

void AActionTestingController::PlayAction()
{
	if (!Action)
	{
		return;
	}
	if (!Action->VerifyContext(this))
	{
		return;
	}
	Action->StartAction();
}

void AActionTestingController::InterruptAction()
{
	if (Action && Action->IsActionRunning())
	{
		Action->AbortAction();
	}
}
