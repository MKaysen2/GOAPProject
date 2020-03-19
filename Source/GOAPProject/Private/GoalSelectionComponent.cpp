#include "../Public/GoalSelectionComponent.h"
#include "../Public/GOAPGoal.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerTypes.h"
#include "GameplayDebuggerCategory.h"
#endif

UGoalSelectionComponent::UGoalSelectionComponent() : Super() 
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.TickInterval = .1f;
}

void UGoalSelectionComponent::OnRegister()
{
	Super::OnRegister();
	CurrentGoal = nullptr;
	NextGoal = nullptr;
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
	for (auto Goal : GoalSet)
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
	bool bGoalChanged = (NextGoal != CurrentGoal);
	CurrentGoal = NextGoal;
	return bGoalChanged;
}

void UGoalSelectionComponent::RegisterGoal(TSubclassOf<UGOAPGoal> GoalClass)
{
	//Null if not a valid subclass
	if (!GoalClass.GetDefaultObject())
	{
		return;
	}
	GoalSet.Add(NewObject<UGOAPGoal>(this, GoalClass));
}

void UGoalSelectionComponent::OnGoalCompleted()
{
	//TODO: deactivate goal here
	CurrentGoal = nullptr;
}

void UGoalSelectionComponent::RegisterGoalSet(const TArray<TSubclassOf<UGOAPGoal>>& NewGoalSet)
{
	for (auto Goal : NewGoalSet)
	{
		RegisterGoal(Goal);
	}
}

#if WITH_GAMEPLAY_DEBUGGER

void UGoalSelectionComponent::DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const
{
	FString CurrentGoalName = (CurrentGoal) ? CurrentGoal->GetName() : TEXT("NONE");
	DebuggerCategory->AddTextLine(FString::Printf(TEXT("Current goal: %s"), *CurrentGoalName));
	DebuggerCategory->AddTextLine(FString::Printf(TEXT("Number of goals: %d"), GoalSet.Num()));
}

#endif //WITH_GAMEPLAY_DEBUGGER