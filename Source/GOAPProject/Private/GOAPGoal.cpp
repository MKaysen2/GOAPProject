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

bool UGOAPGoal::IsGoalValid(AAIController* Controller)
{
	return false;
}

bool UGOAPGoal::IsGoalStillValid(AAIController* Controller)
{
	return false;
}

void UGOAPGoal::ReCalcPriority(AAIController* Controller)
{
}

void UGOAPGoal::Activate(AAIController* Controller)
{
	bIsActive = true;
}

void UGOAPGoal::Deactivate(AAIController* Controller)
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

bool UAIGoal_KillEnemy::IsGoalValid(AAIController* Controller)
{
	UAIPerceptionComponent* PerceptionComponent = Controller->GetPerceptionComponent();
	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	bool bSensedActor = PerceivedActors.Num() > 0;
	CacheValidity(bSensedActor);
	return bSensedActor;
}

void UAIGoal_KillEnemy::Activate(AAIController* Controller)
{
	Super::Activate(Controller);

	/*
	if (CachedTarget == nullptr)
	{
		return;
	}
	UBlackboardComponent* BBComponent = Controller->GetBlackboardComponent();
	BBComponent->SetValueAsObject(FName("Target"), CachedTarget);
	*/
}

void UAIGoal_KillEnemy::ReCalcPriority(AAIController* Controller)
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

UAIGoal_InteractTest::UAIGoal_InteractTest() :
	Super()
{
	LastPriority = 20.0f;
	Goal.Add(FWorldProperty(EWorldKey::kUsingObject, true));
}

bool UAIGoal_InteractTest::IsGoalValid(AAIController* Controller)
{
	UAIPerceptionComponent* PerceptionComponent = Controller->GetPerceptionComponent();
	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	for (auto* Actor : PerceivedActors)
	{
		if (Actor->Implements<UInteractableObjectInterface>())
		{
			Goal[0].Apply(FWorldProperty(EWorldKey::kUsingObject, Actor));
			CacheValidity(true);
			return true;
		}
	}
	CacheValidity(false);
	return false;
}

void UAIGoal_InteractTest::Activate(AAIController* Controller)
{
	Super::Activate(Controller);
}

UAIGoal_Death::UAIGoal_Death()
	: Super()
{
	Goal.Add(FWorldProperty(EWorldKey::kDead, true));
}

bool UAIGoal_Death::IsGoalValid(AAIController* Controller)
{
	return false;
}

void UAIGoal_Death::Activate(AAIController* Controller)
{
	Super::Activate(Controller);
}