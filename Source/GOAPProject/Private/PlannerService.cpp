#include "..\Public\PlannerService.h"
#include "..\Public\PlannerComponent.h"
#include "AIController.h"
#include "WorldProperty.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AISense_Sight.h"

void UPlannerService::TickService(UPlannerComponent& PlannerComp, float DeltaTime)
{
	//empty in base class
}

void UPlanService_TargetProps::TickService(UPlannerComponent& PlannerComp, float DeltaTime)
{
	//empty in base class
	UBlackboardComponent* BBComp = PlannerComp.GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}
	AActor* EnemyActor = Cast<AActor>(BBComp->GetValueAsObject(FName("EnemyActor")));
	if (!EnemyActor)
	{
		TArray<AActor*> OutActors;
		UAIPerceptionComponent* PerceptComp = PlannerComp.GetAIOwner()->GetPerceptionComponent();
		PerceptComp->GetPerceivedHostileActors(OutActors);
		APawn* Pawn = PlannerComp.GetAIOwner()->GetPawn();
		if (OutActors.Num != 0)
		{
			bool bBestActorIsSensed = false;
			float fBestDistSq = TNumericLimits<float>::Max();
			AActor* BestActor = nullptr;
			FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
			//Get the nearest visible target, then nearest nonvisible target
			for (auto* Actor : OutActors)
			{
				
				float newDistSq = FVector::DistSquared(Pawn->GetActorLocation(), PerceptComp->GetActorLocation(*Actor));
				bool bWasSensed = PerceptComp->HasAnyCurrentStimulus(*Actor);
				if (bWasSensed && !bBestActorIsSensed)
				{
					bBestActorIsSensed = bWasSensed;
					fBestDistSq = newDistSq;
					BestActor = Actor;
				}
				else if ((bWasSensed == bBestActorIsSensed) && newDistSq < fBestDistSq)
				{
					fBestDistSq = newDistSq;
					BestActor = Actor;
				}
			}

			if (BestActor)
			{
				BBComp->SetValueAsObject(FName("EnemyActor"), BestActor);
			}
		}
	}

	EnemyActor = Cast<AActor>(BBComp->GetValueAsObject(FName("EnemyActor")));
}