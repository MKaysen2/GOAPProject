#include "..\Public\PlannerService.h"
#include "..\Public\PlannerComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

void UPlannerService::TickService(UPlannerComponent& PlannerComp, float DeltaTime)
{
	//empty in base class
}

void UPlanService_TargetProps::TickService(UPlannerComponent& PlannerComp, float DeltaTime)
{
	//empty in base class
	TArray<AActor*> OutActors;
	PlannerComp.GetAIOwner()->GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), OutActors);
	PlannerComp.SetWSProp(EWorldKey::kIdle, (uint8)OutActors.Num());

	FBlackboard::FKey KeyID = PlannerComp.GetBlackboardComponent()->GetKeyID(FName("EnemyActor"));
	PlannerComp.SetWSProp(EWorldKey::kEnemyActor, KeyID);
}