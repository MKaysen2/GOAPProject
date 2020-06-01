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

EActionStatus UAIAct_DodgeShuffle::StartAction()
{
	if (Super::StartAction() == EActionStatus::kFailed)
	{
		return EActionStatus::kFailed;
	}

	UWorld* World = AIOwner->GetWorld();
	FName TraceTag(TEXT("DodgeQuery"));
	World->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams QueryParams = FCollisionQueryParams(TraceTag, false, AIOwner->GetPawn());
	FHitResult HitResult;
	bool bHitStatic = World->LineTraceSingleByChannel(
		HitResult, 
		AIOwner->GetPawn()->GetActorLocation(), 
		GoalLocation, 
		ECollisionChannel::ECC_WorldStatic,
		QueryParams);

	if (bHitStatic)
	{
		UE_LOG(LogAction, Error, TEXT("Hit a static object %s"), *HitResult.GetActor()->GetName());
		return EActionStatus::kFailed;
	}
	MoveTask = UAITask_MoveTo::AIMoveTo(AIOwner, GoalLocation, nullptr, -1.0f, EAIOptionFlag::Default, EAIOptionFlag::Default, false, false);
	if (!MoveTask)
	{
		UE_LOG(LogAction, Error, TEXT("MoveTo Task was bogus"));
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
	AIOwner->ReceiveMoveCompleted.RemoveAll(this);
	Super::StopAction();
}

void UAIAct_DodgeShuffle::AbortAction()
{
	Super::AbortAction();
	AIOwner->ReceiveMoveCompleted.RemoveAll(this);
	MoveTask = nullptr;
}
