#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WorldProperty.h"
#include "GOAPCharacterBase.generated.h"

class UGOAPAction;
class AAIController;
class UGOAPGoal;
DECLARE_DELEGATE(FTaskEndedSignature);

UCLASS(BlueprintType, Blueprintable)
class GOAPPROJECT_API AGOAPCharacterBase : public ACharacter 
{
	GENERATED_BODY()
protected:

	/* soon...
	UPROPERTY()
		TArray<TSubclassOf<UGOAPGoal>> GoalSet;
		*/
	UPROPERTY()
		TArray<UGOAPAction*> Actions;

public:
	
	AGOAPCharacterBase();

	UFUNCTION()
		TArray<TSubclassOf<UGOAPGoal>> GetGoalSet();

	UFUNCTION()
		TArray<TSubclassOf<UGOAPAction>> GetActionSet();

	UFUNCTION()
	void BeginPlay() override;
	UFUNCTION()
		void PostInitProperties() override;

	FTaskEndedSignature TaskEndedDelegate;

	UFUNCTION(BlueprintCallable)
		void OnTaskEnded(bool bSuccess);
};