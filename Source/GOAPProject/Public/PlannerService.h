#pragma once

#include "CoreMinimal.h"
#include "PlannerService.generated.h"

class UPlannerComponent;
UCLASS(abstract)
class UPlannerService : public UObject
{
	GENERATED_BODY()

public:

	virtual void TickService(UPlannerComponent& PlannerComp, float DeltaTime);

};

UCLASS()
class UPlanService_TargetProps : public UPlannerService
{
	GENERATED_BODY()
public:
	virtual void TickService(UPlannerComponent& PlannerComp, float DeltaTime) override;
};
