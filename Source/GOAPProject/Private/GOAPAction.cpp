

#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\CombatInterface.h"
#include "..\Public\StateNode.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"

UGOAPAction::UGOAPAction() : Super()
{
	
}

UGOAPAction::UGOAPAction(TArray<FWorldProperty>&& Pre, TArray<FWorldProperty>&& Post, int _Cost) :
	Super(),
	preconditions{ Pre },
	effects{ Post },
	edge_cost(_Cost)
{

}

void UGOAPAction::ApplySymbolicEffects(FWorldState& State) const
{
	for (auto& Property : effects)
	{
		State.Apply(Property);
	}
}

void UGOAPAction::UnapplySymbolicEffects(FStateNode* Node) const
{

	for (auto& Prop : effects)
	{
		Node->UnapplyProperty(Prop);
	}
}

void UGOAPAction::AddUnsatisfiedPreconditions(FStateNode* CurrentNode) const
{
	for (auto& Property : preconditions) //by value so should be ok to modify
	{
		CurrentNode->AddPrecondition(Property);
	}
}

void UGOAPAction::StartAction(AAIController* Controller) 
{
	bIsRunning = true;
	//FString action_name = GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());
	if (Character)
	{
		Character->TaskEndedDelegate.BindUObject(this, &UGOAPAction::StopAction, Controller);
	}
}

void UGOAPAction::StopAction(AAIController* Controller) 
{

	FString action_name = GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Stopping action %s"), *action_name));
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());
	if (Character)
	{
		Character->TaskEndedDelegate.Unbind();
	}
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	bIsRunning = false;

	OnActionEnded.ExecuteIfBound();
}

bool UGOAPAction::IsActionRunning() 
{
	return bIsRunning;
}

UAIAct_MoveTo::UAIAct_MoveTo() : 
	Super(
		{},
		{ FWorldProperty(EWorldKey::kAtLocation, EWorldKey::kAtLocation) }, //Whatever the parent specified as the location
		10
	)
{
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


UAIAct_Equip::UAIAct_Equip() : 
	Super(
		{ FWorldProperty(EWorldKey::kAtLocation, EWorldKey::kUsingObject) }, //i.e. we must be at the object to use it
		{ FWorldProperty(EWorldKey::kUsingObject, EWorldKey::kUsingObject)}, //use specified object
		1
	)
{
	
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

UAIAct_ReactDisturbance::UAIAct_ReactDisturbance() :
	Super(
		{},
		{ FWorldProperty(EWorldKey::kDisturbanceHandled, true) },
		1
	)
{
}

bool UAIAct_ReactDisturbance::VerifyContext(AAIController* Controller)
{
	return true;
}

void UAIAct_ReactDisturbance::StartAction(AAIController* Controller)
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
	FTimerManager& TimerMgr = Controller->GetWorldTimerManager();
	FTimerDelegate InDelegate;
	InDelegate.BindUObject(this, &UAIAct_ReactDisturbance::StopAction, Controller);
	TimerMgr.SetTimer(TimerHandle, InDelegate, 3.0f, false);
}

void UAIAct_ReactDisturbance::StopAction(AAIController* Controller)
{
	Controller->GetWorldTimerManager().ClearTimer(TimerHandle);
	Super::StopAction(Controller);
}

UAIAct_CallbackTest::UAIAct_CallbackTest()
	: Super()
{

}

bool UAIAct_CallbackTest::VerifyContext(AAIController* Controller)
{
	return true;
}

void UAIAct_CallbackTest::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);

	if (!Controller)
	{
		return;
	}

	FTimerManager& TimerMgr = Controller->GetWorldTimerManager();
	FTimerDelegate InDelegate;
	InDelegate.BindUObject(this, &UAIAct_CallbackTest::StopAction, Controller);
	TimerMgr.SetTimer(TimerHandle, InDelegate, 3.0f, false);
}

void UAIAct_CallbackTest::StopAction(AAIController* Controller)
{
	Controller->GetWorldTimerManager().ClearTimer(TimerHandle);
	Super::StopAction(Controller);
}