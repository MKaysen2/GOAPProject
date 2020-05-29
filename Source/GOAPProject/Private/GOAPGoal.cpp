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
	Symbols.Add(FWorldProperty(EWorldKey::kTargetDead, true));
}

bool UAIGoal_KillEnemy::IsGoalValid()
{
	if (!AIOwner)
	{
		return false;
	}
	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(FName("CombatTarget")));
	return (TargetActor != nullptr);
}

void UAIGoal_KillEnemy::Activate()
{
	Super::Activate();

	/*
	*/
}

void UAIGoal_KillEnemy::ReCalcPriority()
{
	LastPriority = 10.0f;

}

UAIGoal_Death::UAIGoal_Death()
	: Super()
{
	Symbols.Add(FWorldProperty(EWorldKey::kDead, true));
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
	Symbols.Add(FWorldProperty(EWorldKey::kTargetDead, true));
	LastPriority = 1.0f;
}

bool UAIGoal_AlwaysValid::IsGoalValid()
{
	return IsValid(AIOwner);
}
