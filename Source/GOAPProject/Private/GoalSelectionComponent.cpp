#include "..\Public\GoalSelectionComponent.h"
#include "../Public/GoalSelectionComponent.h"

UGoalPerceptionComponent::UGoalPerceptionComponent() : Super() 
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = .1f;
}

void UGoalPerceptionComponent::OnRegister()
{
	AIOwner = Cast<AAIController>(GetOwner());
}
