

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

EActionStatus UGOAPAction::StartAction() 
{
	bIsRunning = true;
	//FString action_name = GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));
	UE_LOG(LogTemp, Warning, TEXT("Action Started"));

	if (!AIOwner)
	{
		return EActionStatus::kFailed;
	}

	return EActionStatus::kSuccess;
}

void UGOAPAction::StopAction() 
{
	//If we weren't running just don't call anything 
	if (!bIsRunning)
	{
		return;
	}
	UE_LOG(LogAction, Warning, TEXT("Action Stopped"));
	bIsRunning = false;
	OnActionEnded.ExecuteIfBound();
}

void UGOAPAction::AbortAction()
{
	UE_LOG(LogAction, Warning, TEXT("Action Aborted"));
	bIsRunning = false;
	OnActionEnded.Unbind();
}

void UGOAPAction::InitAction(AAIController* Controller)
{
	AIOwner = Controller;
}

bool UGOAPAction::IsActionRunning()
{
	return bIsRunning;
}