#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\WorldState.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

DEFINE_LOG_CATEGORY(LogGoal);

UGOAPGoal::UGOAPGoal() :
	Super(),
	GoalCondition(),
	AIOwner(nullptr),
	Insistence(0.0f),
	bCachedValidity(false)
{
}

void UGOAPGoal::SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent)
{
	AIOwner = &Controller;
	OwnerComp = &OwnerComponent;
	BlackboardKey.SelectedKeyName = KeyName;
	UBlackboardData* BBAsset = AIOwner->GetBlackboardComponent()->GetBlackboardAsset();
	if (BBAsset)
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogGoal, Warning, TEXT("No BB Component"));
	}

}

void UGOAPGoal::OnWSUpdated(const FWorldState& WorldState)
{
	bool bSuccess = true;
	for (auto& Precondition : Preconditions)
	{
		bSuccess = bSuccess && WorldState.CheckCondition(Precondition);
		if (!bSuccess)
		{
			break;
		}
	}
	CacheValidity(bSuccess);

}

float UGOAPGoal::GetInsistence() const
{
	return Insistence;
}