#pragma once

#include "CoreMinimal.h"

#include "PlannerAsset.generated.h"

class UGOAPAction;
class UPlannerComponent;

UCLASS(BlueprintType, Blueprintable)
class GOAPPROJECT_API UPlannerAsset : public UObject
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<UGOAPAction*> Actions;

	friend class UPlannerComponent;
};