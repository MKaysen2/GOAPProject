#include "..\Public\AIAct_Reload.h"
#include "..\Public\CombatInterface.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UAIAct_Reload::UAIAct_Reload() :
	Super(
		{ FWorldProperty(EWorldKey::kHasWeapon, true) },
		{ FWorldProperty(EWorldKey::kWeaponLoaded, true) },
		1
	)
{
}

bool UAIAct_Reload::VerifyContext()
{
	if (!AIOwner)
	{
		return false;
	}
	APawn* Pawn = AIOwner->GetPawn();

	if (!Pawn || !Pawn->Implements<UCombatInterface>())
	{
		return false;
	}
	return true;
}

EActionStatus UAIAct_Reload::StartAction()
{
	Super::StartAction();
	APawn* Pawn = AIOwner->GetPawn();
	bool bInterface = !Pawn || Pawn->Implements<UCombatInterface>();

	//Need to handle failure/Unbind delegates etc
	//Just realized I said almost the exact same thing for VerifyContext
	//TODO: this code should be moved there
	if (!AIOwner || !bInterface)
	{
		return EActionStatus::kFailed;
	}

	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	if (!BBComp)
	{
		return EActionStatus::kFailed;
	}

	//Currently, this is the workaround for not being able to add ActionBPs to the character
	//Once I get around that, then I'll be able to execute animMontages/Movement in the
	//BP event graph directly
	ICombatInterface::Execute_Reload(Pawn);
	return EActionStatus::kRunning;
}