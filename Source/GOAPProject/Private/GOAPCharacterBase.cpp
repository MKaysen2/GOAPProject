#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPGoal.h"
#include "..\Public\AIAct_Attack.h"
#include "..\Public\AIAct_Reload.h"
#include "AIController.h"

AGOAPCharacterBase::AGOAPCharacterBase() : ACharacter() 
{

}

void AGOAPCharacterBase::BeginPlay() 
{
	ACharacter::BeginPlay();

}

void AGOAPCharacterBase::PostInitProperties()
{
	Super::PostInitProperties();
}

TArray<TSubclassOf<UGOAPGoal>> AGOAPCharacterBase::GetGoalSet()
{
	return { UAIGoal_Wander::StaticClass(), UAIGoal_KillEnemy::StaticClass(), UAIGoal_InvestigateNoise::StaticClass() };
}

void AGOAPCharacterBase::RegisterGoals(TArray<TSubclassOf<UGOAPGoal>>& OutArray)
{
	OutArray.Add(UAIGoal_Wander::StaticClass());
	OutArray.Add(UAIGoal_KillEnemy::StaticClass());
	OutArray.Add(UAIGoal_InvestigateNoise::StaticClass());
}

TArray<TSubclassOf<UGOAPAction>> AGOAPCharacterBase::GetActionSet()
{

	return 
	{ 
		UAIAct_Attack::StaticClass(), 
		UAIAct_Equip::StaticClass(), 
		UAIAct_Reload::StaticClass(), 
		UAIAct_ReactDisturbance::StaticClass()
	};
}


void AGOAPCharacterBase::RegisterActions(TArray<TSubclassOf<UGOAPAction>>& OutArray)
{
	/*
	for (auto& Action : Actions)
	{
		if (Action != nullptr)
		{
			OutArray.Add(Action);
		}
	}
	*/
	OutArray.Add(UAIAct_Attack::StaticClass());
	OutArray.Add(UAIAct_Equip::StaticClass());
	OutArray.Add(UAIAct_Reload::StaticClass());
	OutArray.Add(UAIAct_ReactDisturbance::StaticClass());
}


void AGOAPCharacterBase::OnTaskEnded(bool bSuccess)
{
	TaskEndedDelegate.ExecuteIfBound();
}
