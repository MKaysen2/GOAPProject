#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "WorldState.h"
#include "WorldProperty.h"
#include "PlannerComponent.generated.h"

class UGOAPAction;
class UPlannerAsset;
UCLASS()
class GOAPPROJECT_API UPlannerComponent : public UBrainComponent
{
	GENERATED_BODY()
public:
	//unused for now. using messages first
	void OnTaskFinished(UGOAPAction* Action, EPlannerTaskFinishedResult::Type Result);
	void StartPlanner(const UPlannerAsset& PlannerAsset);
	//TODO: move GOAPActionsComp behavior here
protected:

	//this might need to get wrapped in an object so property-specific delegates can be added
	FWorldState WorldState;
	//Also add planner instance

};