#include "../Public/GoalSelectionComponent.h"
#include "../Public/GOAPGoal.h"

UGoalSelectionComponent::UGoalSelectionComponent() : Super() 
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.TickInterval = .1f;
}

void UGoalSelectionComponent::OnRegister()
{
	AIOwner = Cast<AAIController>(GetOwner());
}

UGOAPGoal* UGoalSelectionComponent::GetCurrentGoal()
{
	return CurrentGoal;
}

//TODO: use a Heapified TArray as a PQueue
void UGoalSelectionComponent::ReEvaluateGoals()
{
	NextGoal = nullptr;
	for (auto Goal : Goals)
	{
		if (Goal->IsGoalValid(AIOwner))
		{
			//Recalculate prioirity if necessary
			Goal->ReCalcPriority(AIOwner);

			//Highest priority goal becomes next goal
			if (NextGoal == nullptr)
			{
				NextGoal = Goal;
				continue;
			}
			if (Goal->Priority() > NextGoal->Priority())
				NextGoal = Goal;
		}
	}
}


bool UGoalSelectionComponent::HasGoalChanged()
{
	return NextGoal != CurrentGoal;
}

void UGoalSelectionComponent::RegisterGoal(TSubclassOf<UGOAPGoal> GoalClass)
{
	//Null if not a valid subclass
	if (!GoalClass.GetDefaultObject())
	{
		return;
	}
	Goals.Add(NewObject<UGOAPGoal>(this, GoalClass));
}