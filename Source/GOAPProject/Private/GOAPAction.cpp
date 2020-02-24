

#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\GOAPController.h"

UGOAPAction::UGOAPAction() : Super() 
{
	
}

void UGOAPAction::StartAction(AAIController* controller) 
{
	bIsRunning = true;
	FString action_name = GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));
}


void UGOAPAction::StopAction(AAIController* controller) 
{
	bIsRunning = false;
	FString action_name = GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Stopping action %s"), *action_name));
	OnActionEnded.ExecuteIfBound();
}

bool UGOAPAction::IsActionRunning() 
{
	return bIsRunning;
}
void UGOAPAction::apply_procedural_effects(AAIController* controller) 
{

}

UAIAct_MoveTo::UAIAct_MoveTo() : UGOAPAction()
{
	edge_cost = 10;
	effects.Add(FWorldProperty(EWorldKey::kAtLocation, nullptr));
}

bool UAIAct_MoveTo::VerifyContext(AAIController* Controller) 
{
	return true;
}

void UAIAct_MoveTo::StartAction(AAIController* Controller) 
{
	Super::StartAction(Controller);
	if (!Controller)
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
	ACharacter* Avatar = Cast<ACharacter>(Controller->GetPawn());
	if (!Avatar)
	{
		return;
	}
	FVector Location(Avatar->GetActorLocation());
	FVector Offset(0.0f, -800.0f, 0.0f);
	BBComp->SetValueAsBool(FName("TorsoTracking"), true);

	Controller->MoveToLocation(Location + Offset);
}

UAIAct_Reload::UAIAct_Reload() : UGOAPAction()
{
	edge_cost = 1;
	effects.Add(FWorldProperty(EWorldKey::kWeaponLoaded, true));
	//No effects yet. this is just a test action
}

bool UAIAct_Reload::VerifyContext(AAIController* controller)
{
	//Think this might be fine as is
	return true;
}

void UAIAct_Reload::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	AGOAPController* AIController = Cast<AGOAPController>(Controller);
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());

	if (!AIController || !Character)
	{
		return;
	}
	Character->Reload();
	AIController->SetMontageObservers();

}

UAIAct_Equip::UAIAct_Equip() : Super()
{
	edge_cost = 1;
	effects.Add(FWorldProperty(EWorldKey::kHasWeapon, true));
}

void UAIAct_Equip::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	AGOAPController* AIController = Cast<AGOAPController>(Controller);
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());

	if (!AIController || !Character)
	{
		return;
	}
	
	Character->Equip();
	AIController->SetMontageObservers();

}

UAIAct_Attack::UAIAct_Attack() : Super()
{
	edge_cost = 5;
	preconditions.Add(FWorldProperty(EWorldKey::kWeaponLoaded, true));
	effects.Add(FWorldProperty(EWorldKey::kTargetDead, true));
}

void UAIAct_Attack::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	AGOAPController* AIController = Cast<AGOAPController>(Controller);
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());

	if (!AIController || !Character)
	{
		return;
	}
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("TorsoTracking"), true);
	Character->Attack();
	AIController->SetMontageObservers();
}