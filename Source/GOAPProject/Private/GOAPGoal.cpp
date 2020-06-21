#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPCharacterBase.h"

#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

DEFINE_LOG_CATEGORY(LogGoal);

UGOAPGoal::UGOAPGoal() :
	Super(),
	Symbols(),
	AIOwner(nullptr),
	LastPriority(0.0f),
	bIsActive(false),
	bCachedValidity(false)
{
	Symbols.Empty();
}

const TArray<FWorldProperty>& UGOAPGoal::GetSymbolSet() 
{
	return Symbols;
}

void UGOAPGoal::SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent)
{
	AIOwner = &Controller;
	OwnerComp = &OwnerComponent;
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