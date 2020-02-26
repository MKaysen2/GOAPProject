#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPCharacterBase.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

UGOAPGoal::UGOAPGoal() : Super() 
{
	goal.Empty();
}

const TArray<FWorldProperty>& UGOAPGoal::container() 
{
	return goal;
}

bool UGOAPGoal::IsGoalValid(AAIController* Controller)
{
	return false;
}

void UGOAPGoal::ReCalcPriority(AAIController* Controller)
{
}

void UGOAPGoal::Activate(AAIController* Controller)
{

}
float UGOAPGoal::Priority() const
{
	return LastPriority;
}

UAIGoal_KillEnemy::UAIGoal_KillEnemy() : Super()
{
	LastPriority = 30.0f; //constant value for now
	goal.Add(FWorldProperty(EWorldKey::kTargetDead, true));
}

bool UAIGoal_KillEnemy::IsGoalValid(AAIController* Controller)
{
	UAIPerceptionComponent* PerceptionComponent = Controller->GetPerceptionComponent();
	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	return (PerceivedActors.Num() > 0);
}

void UAIGoal_KillEnemy::Activate(AAIController* Controller)
{
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
	for (auto Actor : PerceivedActors)
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
	UBlackboardComponent* BBComponent = Controller->GetBlackboardComponent();
	BBComponent->SetValueAsObject(FName("Target"), Target);
}

void UAIGoal_KillEnemy::ReCalcPriority(AAIController* Controller)
{
}

UAIGoal_Wander::UAIGoal_Wander() : Super()
{
	LastPriority = 10.0f;
	goal.Add(FWorldProperty(EWorldKey::kAtLocation, true));
}

bool UAIGoal_Wander::IsGoalValid(AAIController* Controller)
{
	return true;
}

void UAIGoal_Wander::Activate(AAIController* Controller)
{

}

void UAIGoal_Wander::ReCalcPriority(AAIController* Controller)
{

}