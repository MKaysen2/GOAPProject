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

void AGOAPCharacterBase::RegisterActions(TArray<UGOAPAction*>& OutArray)
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
	OutArray.Add(NewObject<UAIAct_Equip>(this));
	OutArray.Add(NewObject<UAIAct_Reload>(this));
	OutArray.Add(NewObject<UAIAct_Attack>(this));
	OutArray.Add(NewObject<UAIAct_ReactDisturbance>(this));
}
void AGOAPCharacterBase::RegisterGoals(TArray<UGOAPGoal*>& OutArray)
{
	OutArray.Add(NewObject<UAIGoal_Wander>(this));
	OutArray.Add(NewObject<UAIGoal_KillEnemy>(this));
	OutArray.Add(NewObject<UAIGoal_InvestigateNoise>(this));
}

void AGOAPCharacterBase::OnTaskEnded(bool bSuccess)
{
	TaskEndedDelegate.ExecuteIfBound();
}
