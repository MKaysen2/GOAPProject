#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPCharacterBase.h"

#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

DEFINE_LOG_CATEGORY(LogGoal);

UGOAPGoal::UGOAPGoal() :
	Super(),
	GoalCondition(),
	AIOwner(nullptr),
	Insistence(0.0f),
	bCachedValidity(false)
{
}

void UGOAPGoal::SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent)
{
	AIOwner = &Controller;
	OwnerComp = &OwnerComponent;
}

float UGOAPGoal::GetInsistence() const
{
	return Insistence;
}