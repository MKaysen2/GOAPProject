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

bool UAIAct_Reload::VerifyContext(AAIController* Controller)
{
	if (!Controller)
	{
		return false;
	}
	APawn* Pawn = Controller->GetPawn();

	if (!Pawn || !Pawn->Implements<UCombatInterface>())
	{
		return false;
	}
	return true;
}

EActionStatus UAIAct_Reload::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	APawn* Pawn = Controller->GetPawn();
	bool bInterface = !Pawn || Pawn->Implements<UCombatInterface>();

	//Need to handle failure/Unbind delegates etc
	if (!Controller || !bInterface)
	{
		return EActionStatus::kFailed;
	}

	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
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