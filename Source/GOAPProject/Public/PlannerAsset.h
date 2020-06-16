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

	//Ring buffer for running tasks only needs to be max plan size + 1 (for cancelling actions)
	UPROPERTY(EditDefaultsOnly)
		uint32 MaxPlanSize = 5;
	friend class UPlannerComponent;
};