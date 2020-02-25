#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPGoal.h"

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
	//OutArray.Add(NewObject<UAIAct_Attack>(this));
}
void AGOAPCharacterBase::RegisterGoals(TArray<UGOAPGoal*>& OutArray)
{
	OutArray.Add(NewObject<UAIGoal_KillEnemy>(this));
}

void AGOAPCharacterBase::OnTaskEnded(bool bSuccess)
{
	TaskEndedDelegate.ExecuteIfBound();
}
