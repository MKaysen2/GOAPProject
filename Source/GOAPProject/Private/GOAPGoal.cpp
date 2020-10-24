#include "..\Public\GOAPGoal.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\WorldState.h"
#include "..\Public\GOAPDecorator.h"
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

bool UGOAPGoal::ValidateContextPreconditions(const FWorldState& WS) const
{
	//For now. Will check decorators here
	for (auto* Decorator : Decorators)
	{
		if (!Decorator->CalcRawConditionValue(*AIOwner, WS))
		{
			return false;
		}
	}
	return true;
}

void UGOAPGoal::SetOwner(AAIController& Controller, UPlannerComponent& OwnerComponent)
{
	AIOwner = &Controller;
	OwnerComp = &OwnerComponent;

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