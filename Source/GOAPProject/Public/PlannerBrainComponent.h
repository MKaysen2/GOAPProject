#pragma once

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "WorldState.h"
#include "PlannerBrainComponent.generated.h"


UCLASS()
class UPlannerBrainComponent : public UBrainComponent
{
	GENERATED_BODY()
public:

protected:

	//this might need to get wrapped in an object so property-specific delegates can be added
	FWorldState WorldState;
	//Also add planner instance
};