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

TArray<TSubclassOf<UGOAPGoal>> AGOAPCharacterBase::GetGoalSet()
{
	return {
		UAIGoal_KillEnemy::StaticClass(), 
		//UAIGoal_InteractTest::StaticClass()
	};
}

TArray<TSubclassOf<UGOAPAction>> AGOAPCharacterBase::GetActionSet()
{
	return
	{
	};
}

void AGOAPCharacterBase::OnTaskEnded(bool bSuccess)
{
	TaskEndedDelegate.ExecuteIfBound();
}
