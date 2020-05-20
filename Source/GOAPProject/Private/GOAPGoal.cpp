#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\InteractableObjectInterface.h"

#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

DEFINE_LOG_CATEGORY(LogGoal);

UGOAPGoal::UGOAPGoal() :
	Super(),
	Goal(),
	AIOwner(nullptr),
	LastPriority(0.0f),
	bIsActive(false),
	bCachedValidity(false)
{
	Goal.Empty();
}

const TArray<FWorldProperty>& UGOAPGoal::container() 
{
	return Goal;
}

void UGOAPGoal::InitGoal(AAIController* Controller)
{
	AIOwner = Controller;
}

bool UGOAPGoal::IsGoalValid()
{
	return false;
}

bool UGOAPGoal::IsGoalStillValid()
{
	return false;
}

void UGOAPGoal::ReCalcPriority()
{
}

void UGOAPGoal::Activate()
{
	bIsActive = true;
}

void UGOAPGoal::Deactivate()
{

	bIsActive = false;
}

float UGOAPGoal::Priority() const
{
	return LastPriority;
}

UAIGoal_KillEnemy::UAIGoal_KillEnemy() : Super()
{
	LastPriority = 0.0f; 
	Goal.Add(FWorldProperty(EWorldKey::kTargetDead, true));
}

bool UAIGoal_KillEnemy::IsGoalValid()
{
	if (!AIOwner)
	{
		return false;
	}
	UAIPerceptionComponent* PerceptionComponent = AIOwner->GetPerceptionComponent();
	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	bool bSensedActor = PerceivedActors.Num() > 0;
	CacheValidity(bSensedActor);
	return bSensedActor;
}

void UAIGoal_KillEnemy::Activate()
{
	Super::Activate();

	/*
	if (CachedTarget == nullptr)
	{
		return;
	}
	UBlackboardComponent* BBComponent = Controller->GetBlackboardComponent();
	BBComponent->SetValueAsObject(FName("Target"), CachedTarget);
	*/
}

void UAIGoal_KillEnemy::ReCalcPriority()
{
	LastPriority = 10.0f;
	/*
	if (!Controller)
	{
		return;
	}
	UAIPerceptionComponent* PerceptionComponent = Controller->GetPerceptionComponent();

	APawn* ControlledPawn = Controller->GetPawn();
	if (!ControlledPawn || !PerceptionComponent)
	{
		return;
	}

	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	AActor* Target = nullptr;
	FVector Location(ControlledPawn->GetActorLocation());
	float TargetDistance = 0.0f;
	for (auto* Actor : PerceivedActors)
	{
		if (!Actor)
		{
			continue;
		}

		if (!Target)
		{
			Target = Actor;
			TargetDistance = ControlledPawn->GetDistanceTo(Target);
		}
		else
		{
			float TestDistance = ControlledPawn->GetDistanceTo(Actor);
			if (TestDistance < TargetDistance)
			{
				Target = Actor;
				TargetDistance = TestDistance;
			}
		}
	}
	if (!Target)
	{
		return;
	}
	LastPriority = TargetDistance;
	CachedTarget = Target;*/
}

UAIGoal_Death::UAIGoal_Death()
	: Super()
{
	Goal.Add(FWorldProperty(EWorldKey::kDead, true));
}

bool UAIGoal_Death::IsGoalValid()
{
	return false;
}

void UAIGoal_Death::Activate()
{
	Super::Activate();
}

UAIGoal_AlwaysValid::UAIGoal_AlwaysValid() : Super()
{
	Goal.Add(FWorldProperty(EWorldKey::kTargetDead, true));
	LastPriority = 1.0f;
}

bool UAIGoal_AlwaysValid::IsGoalValid()
{
	return IsValid(AIOwner);
}
