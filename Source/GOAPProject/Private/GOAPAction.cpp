

#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\GOAPController.h"

UGOAPAction::UGOAPAction() : Super() 
{
	
}

void UGOAPAction::start_action(AAIController* controller) 
{
	bIsRunning = true;
	FString action_name = GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));
}


void UGOAPAction::stop_action(AAIController* controller) 
{
	bIsRunning = false;
	FString action_name = GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Stopping action %s"), *action_name));

}

bool UGOAPAction::IsActionRunning() {
	return bIsRunning;
}
void UGOAPAction::apply_procedural_effects(AAIController* controller) {

}

UActionMoveTo::UActionMoveTo() : UGOAPAction(), times_run(0)
{
	edge_cost = 10;
	effects.Add(FWorldProperty(EWorldKey::kAtLocation, nullptr));
}

bool UActionMoveTo::verify_context(AAIController* Controller) {
	return true;
}

void UActionMoveTo::start_action(AAIController* controller) 
{

	Super::start_action(controller);
	++times_run;
}

UAIAct_Reload::UAIAct_Reload() : UGOAPAction()
{
	edge_cost = 1;
	effects.Add(FWorldProperty(EWorldKey::kWeaponLoaded, true));
	//No effects yet. this is just a test action
}

bool UAIAct_Reload::verify_context(AAIController* controller)
{
	//Think this might be fine as is
	return true;
}

void UAIAct_Reload::start_action(AAIController* Controller)
{
	Super::start_action(Controller);
	AGOAPController* AIController = Cast<AGOAPController>(Controller);
	if (!AIController)
	{
		return;
	}
	AIController->SetAnimState(EAnimState::Anim);
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(AIController->GetPawn());
	if (!Character)
	{
		return;
	}
	Character->Reload();
}

UAIAct_Equip::UAIAct_Equip() : Super()
{
	edge_cost = 1;
	effects.Add(FWorldProperty(EWorldKey::kHasWeapon, true));
}

void UAIAct_Equip::start_action(AAIController* Controller)
{
	Super::start_action(Controller);
	AGOAPController* AIController = Cast<AGOAPController>(Controller);
	if (!AIController)
	{
		return;
	}
	AIController->SetAnimState(EAnimState::Anim);
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(AIController->GetPawn());
	if (!Character)
	{
		return;
	}
	Character->Equip();
}

UAIAct_Attack::UAIAct_Attack() : Super()
{
	edge_cost = 5;
	preconditions.Add(FWorldProperty(EWorldKey::kWeaponLoaded, true));
	effects.Add(FWorldProperty(EWorldKey::kTargetDead, true));
}

void UAIAct_Attack::start_action(AAIController* Controller)
{
	Super::start_action(Controller);
	AGOAPController* AIController = Cast<AGOAPController>(Controller);
	if (!AIController)
	{
		return;
	}
	AIController->SetAnimState(EAnimState::Anim);
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(AIController->GetPawn());
	if (!Character)
	{
		return;
	}
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("TorsoTracking"), true);
	Character->Attack();
}