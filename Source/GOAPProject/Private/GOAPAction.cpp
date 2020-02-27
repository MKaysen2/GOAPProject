

#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\CombatInterface.h"
#include "..\Public\WorldState.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"

UGOAPAction::UGOAPAction() : Super() 
{
	
}

void UGOAPAction::ApplySymbolicEffects(UWorldState* State) const
{
	for (auto& prop : effects)
	{
		State->apply_effect(prop);
	}
}

int UGOAPAction::UnapplySymbolicEffects(UWorldState* CurrentState, const UWorldState* GoalState) const
{
	//TODO: solve variable properties by getting value indicated by prop
	int satisfied = 0;
	for (auto& Prop : effects)
	{
		CurrentState->TrySatisfyPropertyFrom(GoalState, Prop);
	}
	return satisfied;
}

void UGOAPAction::AddUnsatisfiedPreconditions(UWorldState* CurrentState, const UWorldState* GoalState) const
{
	for (auto Property : preconditions) //by value so should be ok to modify
	{
		CurrentState->AddPropertyAndTrySatisfy(GoalState, Property);
	}
}

void UGOAPAction::StartAction(AAIController* Controller) 
{
	bIsRunning = true;
	FString action_name = GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());
	if (Character)
	{
		Character->TaskEndedDelegate.BindUObject(this, &UGOAPAction::StopAction, Controller);
	}
}

void UGOAPAction::StopAction(AAIController* Controller) 
{

	FString action_name = GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Stopping action %s"), *action_name));
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());
	if (Character)
	{
		Character->TaskEndedDelegate.Unbind();
	}
	Controller->SetFocus(nullptr);
	bIsRunning = false;

	OnActionEnded.ExecuteIfBound();
}

bool UGOAPAction::IsActionRunning() 
{
	return bIsRunning;
}

UAIAct_MoveTo::UAIAct_MoveTo() : UGOAPAction()
{
	edge_cost = 10;
	effects.Add(FWorldProperty(EWorldKey::kAtLocation, true));
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

	Controller->MoveToLocation(Location + Offset);
}


UAIAct_Equip::UAIAct_Equip() : Super()
{
	edge_cost = 1;
	effects.Add(FWorldProperty(EWorldKey::kAtLocation, true));
}

void UAIAct_Equip::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);

	APawn* Pawn = Controller->GetPawn();
	bool bInterface = Pawn->Implements<UCombatInterface>();

	//Need to handle failure/Unbind delegates etc
	if (!Controller )
	{
		return;
	}

	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}
	Controller->SetFocus(nullptr);
	//Currently, this is the workaround for not being able to add ActionBPs to the character
	//Once I get around that, then I'll be able to execute animMontages/Movement in the
	//BP event graph directly
	ICombatInterface::Execute_Equip(Pawn);

}
