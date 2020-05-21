// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/AIAct_DodgeShuffle.h"
#include "AIModule/Classes/Tasks/AITask_MoveTo.h"
#include "../Public/WorldProperty.h"
#include "AIController.h"

UAIAct_DodgeShuffle::UAIAct_DodgeShuffle() :
	Super(
		{},
		{ FWorldProperty(EWorldKey::kInDanger, false)},
		5
	)
{
	GoalLocation = FAISystem::InvalidLocation;
}

bool UAIAct_DodgeShuffle::VerifyContext(AAIController* Controller)
{
	if (!Controller)
	{
		return false;
	}
	APawn* Pawn = Controller->GetPawn();

	if (!Pawn)
	{
		return false;
	}

	GoalLocation = Pawn->GetActorLocation() + (Pawn->GetActorRightVector() * 300.0f);
	
	return true;
}

EActionStatus UAIAct_DodgeShuffle::StartAction()
{
	UWorld* World = AIOwner->GetWorld();
	FHitResult HitResult;
	bool bHitStatic = World->LineTraceSingleByChannel(HitResult, AIOwner->GetPawn()->GetActorLocation(), GoalLocation, ECollisionChannel::ECC_WorldStatic);
	if (bHitStatic)
	{
		return EActionStatus::kFailed;
	}
	MoveTask = UAITask_MoveTo::AIMoveTo(AIOwner, GoalLocation, nullptr, -1.0f, EAIOptionFlag::Default, EAIOptionFlag::Default, false, false);
	if (!MoveTask)
	{
		return EActionStatus::kFailed;
	}
	AIOwner->ReceiveMoveCompleted.AddDynamic(this, &UAIAct_DodgeShuffle::OnMoveCompleted);
	MoveTask->ReadyForActivation();
	return EActionStatus::kRunning;
}

void UAIAct_DodgeShuffle::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	StopAction();
}

void UAIAct_DodgeShuffle::StopAction()
{
	MoveTask = nullptr;
	Super::StopAction();
}

void UAIAct_DodgeShuffle::AbortAction()
{
	Super::AbortAction();
	AIOwner->ReceiveMoveCompleted.RemoveAll(this);
	MoveTask = nullptr;
}
