#include "..\Public\AIAct_Attack.h"
#include "..\Public\CombatInterface.h"
#include "AIController.h"
#include "..\Public\WorldProperty.h"
#include "..\Public\WorldState.h"
#include "GameFramework/Pawn.h"

UAIAct_Attack::UAIAct_Attack() : Super()
{
	edge_cost = 5;
	preconditions.Add(FWorldProperty(EWorldKey::kWeaponLoaded, true));
	effects.Add(FWorldProperty(EWorldKey::kTargetDead, true));
}

bool UAIAct_Attack::VerifyContext(AAIController* Controller)
{

	if (!Controller)
	{
		return false;
	}
	APawn* Pawn = Controller->GetPawn();

	//Need to handle failure/Unbind delegates etc
	if (!Pawn->Implements<UCombatInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Interface not impl"));
		return false;
	}
	return true;
}
void UAIAct_Attack::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	APawn* Pawn = Controller->GetPawn();
	bool bInterface = Pawn->Implements<UCombatInterface>();

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
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(FName("Target")));
	if (TargetActor)
	{
		Controller->SetFocus(TargetActor);
	}

	//Currently, this is the workaround for not being able to add ActionBPs to the character
	//Once I get around that, then I'll be able to execute animMontages/Movement in the
	//BP event graph directly
	ICombatInterface::Execute_Attack(Pawn);
}