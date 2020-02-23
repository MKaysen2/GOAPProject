#include "..\Public\GOAPCharacterBase.h"

AGOAPCharacterBase::AGOAPCharacterBase() : ACharacter() 
{

}

void AGOAPCharacterBase::BeginPlay() {
	ACharacter::BeginPlay();

}

void AGOAPCharacterBase::PostInitProperties()
{
	Super::PostInitProperties();
}

void AGOAPCharacterBase::OnAnimState(const EAnimProps& AnimToPlay)
{
}

void AGOAPCharacterBase::RegisterGoals(TArray<UGOAPGoal*>& OutArray)
{
	OutArray.Add(NewObject<UAIGoal_KillEnemy>(this));
}
