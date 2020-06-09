

#include "..\Public\GOAPAction.h"

#include "..\Public\StateNode.h"

#include "AIController.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogAction);

const FName MontageCompleted = TEXT("MontageCompleted");
const FName MontageBlendingOut = TEXT("MontageBlendingOut");

UGOAPAction::UGOAPAction() : Super()
{
	
}

UGOAPAction::UGOAPAction(const int& Cost) :
	Super(),
	Preconditions(),
	Effects(),
	EdgeCost(Cost)
{
}

void UGOAPAction::AddEffect(const EWorldKey& Key, const FAISymEffect& Effect)
{
	Effects.Add(Effect);
}

void UGOAPAction::AddPrecondition(const EWorldKey& Key, const uint8& Value)
{
	Preconditions.Add({ Key, Value });
}

void UGOAPAction::ApplySymbolicEffects(FWorldState& State) const
{
}

void UGOAPAction::InitPreconditions()
{
}

void UGOAPAction::InitEffects()
{
}

EActionStatus UGOAPAction::StartAction() 
{
	bIsRunning = true;
	//FString action_name = GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Starting action %s"), *action_name));

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
	bIsRunning = false;
	OnActionEnded.ExecuteIfBound();
}

void UGOAPAction::AbortAction()
{
	bIsRunning = false;
	OnActionEnded.Unbind();
}

void UGOAPAction::InitAction(AAIController* Controller)
{
	AIOwner = Controller;
	InitPreconditions();
	InitEffects();
}

bool UGOAPAction::IsActionRunning()
{
	return bIsRunning;
}