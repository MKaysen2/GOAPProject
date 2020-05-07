

#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\CombatInterface.h"
#include "..\Public\StateNode.h"
#include "../Public/GOAPController.h"

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

void UGOAPAction::SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState>)
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

void UAIAct_MoveTo::SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState> Context)
{
	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();

	if (!Context.IsValid() || !BBComp)
	{
		return;
	}
	const FWorldProperty& Prop = Context->GetProperty(EWorldKey::kAtLocation);
	if (Prop.DataType != FWorldProperty::Type::kObj)
	{
		return;
	}
	AActor* Target = Cast<AActor>(Prop.Data.objValue);
	if (!IsValid(Target))
	{
		return;
	}

	BBComp->SetValueAsObject(FName("Target"), Target);
}

void UAIAct_MoveTo::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result, AAIController* Controller)
{
	
	AGOAPController* GOAPOwner = Cast<AGOAPController>(Controller);

	if (GOAPOwner && Result.IsSuccess())
	{
		TSharedPtr<FWorldState> WS = GOAPOwner->GetWorldState();
		UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
		if (WS.IsValid() && BBComp)
		{
			UObject* Target = BBComp->GetValueAsObject(FName("Target"));
			FWorldProperty Prop(EWorldKey::kAtLocation, Target);
			WS->Apply(Prop);
		}
	}
	this->StopAction(Controller);
}

bool UAIAct_MoveTo::VerifyContext(AAIController* Controller)
{
	return true;
}

void UAIAct_MoveTo::StartAction(AAIController* Controller) 
{
	bIsRunning = true;
	if (!Controller)
	{
		return;
	}
	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
	UPathFollowingComponent* PathComp = Controller->GetPathFollowingComponent();

	if (!BBComp || !PathComp)
	{
		return;
	}
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(FName("Target")));
	if (!TargetActor)
	{
		return;
	}
	
	Controller->SetFocus(TargetActor);

	MoveHandle = PathComp->OnRequestFinished.AddUObject(this, &UAIAct_MoveTo::OnMoveCompleted, Controller);
	Controller->MoveToActor(TargetActor);

}

void UAIAct_MoveTo::StopAction(AAIController* Controller)
{
	if (Controller)
	{
		Controller->ClearFocus(EAIFocusPriority::Gameplay);

		UPathFollowingComponent* PathComp = Controller->GetPathFollowingComponent();
		if (PathComp)
		{
			PathComp->OnRequestFinished.Remove(MoveHandle);
		}
	}
	MoveHandle.Reset();

	bIsRunning = false;

	OnActionEnded.ExecuteIfBound();
}


UAIAct_Equip::UAIAct_Equip() : 
	Super(
		{ FWorldProperty(EWorldKey::kAtLocation, EWorldKey::kUsingObject) }, //i.e. we must be at the object to use it
		{ FWorldProperty(EWorldKey::kUsingObject, EWorldKey::kUsingObject)}, //use specified object
		1
	)
{
	
}

void UAIAct_Equip::SetBBTargets(AAIController* Controller, TSharedPtr<FWorldState> Context)
{
	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();

	if (!Context.IsValid() || !BBComp)
	{
		return;
	}
	const FWorldProperty& Prop = Context->GetProperty(EWorldKey::kAtLocation);
	if (Prop.DataType != FWorldProperty::Type::kObj)
	{
		return;
	}
	AActor* Target = Cast<AActor>(Prop.Data.objValue);
	if (!IsValid(Target))
	{
		return;
	}

	BBComp->SetValueAsObject(FName("Target"), Target);
}

void UAIAct_Equip::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	UE_LOG(LogTemp, Warning, TEXT("Equip::StartAction Called"));

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


	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(FName("Target")));
	if (!TargetActor)
	{
		return;
	}

	Controller->SetFocus(TargetActor);
	//Currently, this is the workaround for not being able to add ActionBPs to the character
	//Once I get around that, then I'll be able to execute animMontages/Movement in the
	//BP event graph directly
	ICombatInterface::Execute_Equip(Pawn);

}

void UAIAct_Equip::StopAction(AAIController* Controller)
{

	UE_LOG(LogTemp, Warning, TEXT("Equip::StopAction Called"));
	Super::StopAction(Controller);

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