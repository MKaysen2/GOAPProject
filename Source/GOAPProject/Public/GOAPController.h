#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception\AIPerceptionTypes.h"
#include "AITypes.h"
#include "BrainComponent.h"
#include "GOAPController.generated.h"



class UAStarComponent;
class UWorldState;
class UGOAPGoal;
class UGOAPAction;
class UGOAPActionsComponent;
class UAISenseConfig_Sight;

//I really need a messaging system somewhere
//Maybe I can extend BrainComponent and use theirs


UCLASS(BlueprintType)
class GOAPPROJECT_API AGOAPController : public AAIController 
{

	GENERATED_BODY()
private:
	UPROPERTY()
		UAStarComponent* AStarComponent;
	UPROPERTY()
		UAISenseConfig_Sight* sightConfig;
		
	UPROPERTY()
		UWorldState* current_state;

	UPROPERTY()
	UGOAPGoal* current_goal;
	UPROPERTY()
		UGOAPGoal* NextGoal;
	//This should (probably) not be getting changed after possession
	UPROPERTY()
		TArray<UGOAPGoal*> Goals;
	
	UPROPERTY()
		TArray<UGOAPAction*> ActionSet;

	//Should this go in the Pawn, or the Controller?
	UPROPERTY()
		UGOAPActionsComponent* GOAPActionsComponent;


public:
	AGOAPController();
	static const FName DamageMsg;

	FAIMessageObserverHandle TestObserverHandle;
	FOnAIMessage AIMessageDelegate;
	UFUNCTION()
		void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void OnPossess(APawn* InPawn) override;
	UFUNCTION()
		void OnUnPossess() override;

	UFUNCTION()
		void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	//TODO: move this into a separate componennt
	UFUNCTION()
		void ReEvaluateGoals();	
	UFUNCTION()
		bool HasGoalChanged();

	UFUNCTION()
		void RePlan();

		void OnDamageReceived(UBrainComponent* BrainComp, const FAIMessage& Message);

	UFUNCTION()
		bool IsPlayingMontage();

	UFUNCTION()
		void OnPlanCompleted();
};