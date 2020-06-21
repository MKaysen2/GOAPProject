#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "WorldState.h"
#include "AITypes.h"
#include "BrainComponent.h"
#include "Templates/SharedPointer.h"
#include "GOAPController.generated.h"



class UAStarComponent;
class UGOAPGoal;
class UGOAPAction;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

//I really need a messaging system somewhere
//Maybe I can extend BrainComponent and use theirs


UCLASS(BlueprintType)
class GOAPPROJECT_API AGOAPController : public AAIController 
{

	GENERATED_BODY()
private:

	/////////////////////////////////////////////////////
	// GOAP related components and members

	///////////////////////////////////////
	//Sense configs
	UPROPERTY()
		UAISenseConfig_Sight* sightConfig;

	UPROPERTY()
		UAISenseConfig_Hearing* HearingConfig;

public:
	AGOAPController();
	static const FName DamageMsg;

	UFUNCTION()
		void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void OnPossess(APawn* InPawn) override;
	UFUNCTION()
		void OnUnPossess() override;

	UFUNCTION()
		void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
		void RePlan();

		void OnDamageReceived(UBrainComponent* BrainComp, const FAIMessage& Message);

		//TODO: move to component
		UFUNCTION(BlueprintCallable)
			void ApplyWorldProp(const EWorldKey Key, const bool bValue);
	UFUNCTION()
		bool IsPlayingMontage();

	UFUNCTION()
		void OnGoalChanged(UGOAPGoal* NewGoal);

	UFUNCTION()
		void OnPlanCompleted();
};