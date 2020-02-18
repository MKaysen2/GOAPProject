#pragma once
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "GoalSelectionComponent.generated.h"

UCLASS(BlueprintType)
class GOAPPROJECT_API UGoalPerceptionComponent : public UActorComponent 
{
	GENERATED_BODY()
protected:
	UPROPERTY(Transient)
		AAIController* AIOwner;

public:
	UGoalPerceptionComponent();
	UFUNCTION()
		virtual void OnRegister() override;
};
