// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/AIAct_DodgeShuffle.h"
#include "AIModule/Classes/Tasks/AITask_MoveTo.h"
#include "../Public/WorldProperty.h"
#include "AIController.h"

UAIAct_DodgeShuffle::UAIAct_DodgeShuffle() :
	Super(5)
{
	GoalLocation = FAISystem::InvalidLocation;
}

bool UAIAct_DodgeShuffle::VerifyContext()
{
	if (!AIOwner)
	{
		return false;
	}
	APawn* Pawn = AIOwner->GetPawn();

	if (!Pawn)
	{
		return false;
	}

	GoalLocation = Pawn->GetActorLocation() + (Pawn->GetActorRightVector() * 100.0f);
	
	return true;
}