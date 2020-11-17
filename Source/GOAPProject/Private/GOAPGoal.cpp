#include "..\Public\GOAPGoal.h"
#include "..\Public\WorldState.h"
#include "..\Public\GOAPDecorator.h"
#include "BehaviorTree/BlackboardData.h"
#include "AIController.h"

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

void UGOAPGoal::OnPlanFinished()
{
	if (!OwnerComp)
	{
		return;
	}
	for (auto* Decorator : Decorators)
	{
		Decorator->OnTaskDeactivated(*OwnerComp);
	}
}

float UGOAPGoal::GetInsistence() const
{
	return Insistence;
}