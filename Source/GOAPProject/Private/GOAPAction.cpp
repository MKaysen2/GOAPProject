

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

DEFINE_LOG_CATEGORY(LogAction);

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

EActionStatus UGOAPAction::StartAction(AAIController* Controller) 
{
	bIsRunning = true;
	//FString action_name = GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));
	//TODO: Chagne AGOAPCharacterBase to ActionOwnerInterface
	if (!Controller)
	{
		return EActionStatus::kFailed;
	}
	AGOAPCharacterBase* Character = Cast<AGOAPCharacterBase>(Controller->GetPawn());
	if (!Character)
	{
		return EActionStatus::kFailed;

	}
	return EActionStatus::kSuccess;
}

void UGOAPAction::StopAction(AAIController* Controller) 
{
	//If we weren't running just don't call anything 
	if (!bIsRunning)
	{
		return;
	}
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	bIsRunning = false;

	OnActionEnded.ExecuteIfBound();
}

void UGOAPAction::AbortAction(AAIController* Controller)
{
	bIsRunning = false;
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	OnActionEnded.Unbind();
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

EActionStatus UAIAct_MoveTo::StartAction(AAIController* Controller) 
{
	bIsRunning = true;
	if (!Controller)
	{
		return EActionStatus::kFailed;
	}
	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
	UPathFollowingComponent* PathComp = Controller->GetPathFollowingComponent();

	if (!BBComp || !PathComp)
	{
		return EActionStatus::kFailed;
	}
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(FName("Target")));
	if (!TargetActor)
	{
		return EActionStatus::kFailed;
	}
	
	Controller->SetFocus(TargetActor);

	MoveHandle = PathComp->OnRequestFinished.AddUObject(this, &UAIAct_MoveTo::OnMoveCompleted, Controller);
	Controller->MoveToActor(TargetActor);
	return EActionStatus::kRunning;
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

EActionStatus UAIAct_Equip::StartAction(AAIController* Controller)
{
	Super::StartAction(Controller);
	UE_LOG(LogTemp, Warning, TEXT("Equip::StartAction Called"));

	APawn* Pawn = Controller->GetPawn();
	bool bInterface = Pawn->Implements<UCombatInterface>();

	//Need to handle failure/Unbind delegates etc
	if (!Controller )
	{
		return EActionStatus::kFailed;
	}

	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
	if (!BBComp)
	{
		return EActionStatus::kFailed;
	}


	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(FName("Target")));
	if (!TargetActor)
	{
		return EActionStatus::kFailed;
	}

	Controller->SetFocus(TargetActor);
	//Currently, this is the workaround for not being able to add ActionBPs to the character
	//Once I get around that, then I'll be able to execute animMontages/Movement in the
	//BP event graph directly
	ICombatInterface::Execute_Equip(Pawn);
	return EActionStatus::kRunning;
}

void UAIAct_Equip::StopAction(AAIController* Controller)
{

	UE_LOG(LogTemp, Warning, TEXT("Equip::StopAction Called"));
	Super::StopAction(Controller);

}