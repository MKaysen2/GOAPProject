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
	return {
		//UAIGoal_Wander::StaticClass(), 
		//UAIGoal_KillEnemy::StaticClass(), 
		//UAIGoal_InvestigateNoise::StaticClass(),
		UAIGoal_InteractTest::StaticClass()
	};
}

TArray<TSubclassOf<UGOAPAction>> AGOAPCharacterBase::GetActionSet()
{

	return
	{
		//UAIAct_Attack::StaticClass(), 
		UAIAct_Equip::StaticClass(),
		UAIAct_MoveTo::StaticClass()
		//UAIAct_Reload::StaticClass(), 
		//UAIAct_ReactDisturbance::StaticClass()
	};
}

void AGOAPCharacterBase::OnTaskEnded(bool bSuccess)
{
	TaskEndedDelegate.ExecuteIfBound();
}
