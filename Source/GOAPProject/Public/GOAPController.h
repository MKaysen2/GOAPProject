#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "AStarComponent.h"
#include "GOAPAction.h"
#include "GOAPGoal.h"
#include "Perception\AISenseConfig_Sight.h"
#include "GOAPCharacterBase.h"
#include "BehaviorTree\BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Animation/AnimInstance.h"
#include "Delegates/Delegate.h"
#include "Containers/Queue.h"
#include "Perception\AIPerceptionTypes.h"
#include "GOAPActionsComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "GOAPController.generated.h"

UENUM(BlueprintType)
enum class EAnimState : uint8 {
	Move,
	Anim
};
//I really need a messaging system somewhere
//Maybe I can extend BrainComponent and use theirs

UCLASS(BlueprintType)
class GOAPPROJECT_API AGOAPController : public AAIController {

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

	UPROPERTY()
		EAnimState AnimState;

public:
	AGOAPController();

	UFUNCTION()
		void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void OnPossess(APawn* InPawn) override;
	UFUNCTION()
		void OnUnPossess() override;

	UFUNCTION()
		void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	//TODO: move this into a separate componennt
	UFUNCTION()
		void ReEvaluateGoals();
	UFUNCTION()
		bool HasGoalChanged();

	UFUNCTION()
		void RePlan();

	UFUNCTION()
		bool IsInState(EAnimState CheckState);
	UFUNCTION()
		bool IsPlayingMontage();
	//Called when actions are started to set the state
	UFUNCTION()
		void SetAnimState(const EAnimState& NewState);
};