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
		PlannerComp.GetAIOwner()->GetPerceptionComponent()->GetPerceivedHostileActors(OutActors);

		ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(FGenericTeamId(2), FGenericTeamId(10));
		
		if (OutActors.Num() != 0)
		{
			BBComp->SetValueAsObject(FName("EnemyActor"), OutActors[0]);
		}
	}

}