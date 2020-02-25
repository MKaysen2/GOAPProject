#include "..\Public\AIAct_Reload.h"
#include "..\Public\CombatInterface.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UAIAct_Reload::UAIAct_Reload() : UGOAPAction()
{
	edge_cost = 1;
	effects.Add(FWorldProperty(EWorldKey::kWeaponLoaded, true));
	//No effects yet. this is just a test action
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

void UAIAct_Reload::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	APawn* Pawn = Controller->GetPawn();
	bool bInterface = !Pawn || Pawn->Implements<UCombatInterface>();

	//Need to handle failure/Unbind delegates etc
	if (!Controller || !bInterface)
	{
		return;
	}

	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}

	//Currently, this is the workaround for not being able to add ActionBPs to the character
	//Once I get around that, then I'll be able to execute animMontages/Movement in the
	//BP event graph directly
	ICombatInterface::Execute_Reload(Pawn);
}