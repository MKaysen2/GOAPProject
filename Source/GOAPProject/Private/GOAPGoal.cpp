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

bool UGOAPGoal::IsValid(AAIController* Controller)
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

bool UAIGoal_KillEnemy::IsValid(AAIController* Controller)
{
	UBlackboardComponent* BBComponent = Controller->GetBlackboardComponent();
	UObject* Target = BBComponent->GetValueAsObject(FName("Target"));
	return (Target !=nullptr);
}

void UAIGoal_KillEnemy::Activate(AAIController* Controller)
{

}

void UAIGoal_KillEnemy::ReCalcPriority(AAIController* Controller)
{
}