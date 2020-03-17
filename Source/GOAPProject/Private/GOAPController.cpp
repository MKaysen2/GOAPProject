#include "..\Public\GOAPController.h"
#include "..\Public\GOAPCharacterBase.h"
#include "..\Public\GOAPActionsComponent.h"
#include "..\Public\AStarComponent.h"
#include "..\Public\GOAPAction.h"
#include "..\Public\GOAPGoal.h"
#include "..\Public\WorldState.h"
#include "Perception\AISenseConfig_Sight.h"
#include "Perception\AISenseConfig_Hearing.h"

#include "BehaviorTree\BlackboardComponent.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Animation/AnimInstance.h"
#include "Delegates/Delegate.h"
#include "Containers/Queue.h"
#include "Perception/AIPerceptionComponent.h"

const FName AGOAPController::DamageMsg = TEXT("DamageMsg");

AGOAPController::AGOAPController()
{
	AStarComponent = CreateDefaultSubobject<UAStarComponent>(TEXT("AStarComponent"));
	
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	PerceptionComponent->ConfigureSense(*sightConfig);
	
	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGOAPController::TargetPerceptionUpdated);

	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	UBlackboardData* BBAsset = NewObject<UBlackboardData>(Blackboard);
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("Target"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("TargetLocation"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("FaceTracking"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("TorsoTracking"));

	Blackboard->InitializeBlackboard(*BBAsset);

	BrainComponent = CreateDefaultSubobject<UBrainComponent>(TEXT("BrainComponent"));
	AIMessageDelegate.BindUObject(this, &AGOAPController::OnDamageReceived);

	current_state = CreateDefaultSubobject<UWorldState>(TEXT("ControllerState"));
	current_state->add_property(FWorldProperty(EWorldKey::kWeaponLoaded, false));
	current_state->add_property(FWorldProperty(EWorldKey::kHasWeapon, false));
	current_state->add_property(FWorldProperty(EWorldKey::kIdle, false));
	current_state->add_property(FWorldProperty(EWorldKey::kAtLocation, false));
	current_state->add_property(FWorldProperty(EWorldKey::kTargetDead, false));
	current_state->add_property({ EWorldKey::kDisturbanceHandled, false });


	current_goal = nullptr;
	GOAPActionsComponent = CreateDefaultSubobject<UGOAPActionsComponent>(TEXT("GOAPActionsComp"));
	GOAPActionsComponent->OnPlanCompleted.BindUObject(this, &AGOAPController::OnPlanCompleted);
}

void AGOAPController::OnPossess(APawn * InPawn)
{
	Super::OnPossess(InPawn);
	sightConfig->SightRadius = 800.0f;
	sightConfig->SetMaxAge(5.0f);
	sightConfig->LoseSightRadius = (800.0f + 30.0f);
	sightConfig->PeripheralVisionAngleDegrees = 45.0f;
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	
	HearingConfig->HearingRange = 600.0f;
	HearingConfig->SetMaxAge(5.0f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComponent->ConfigureSense(*sightConfig);

	TestObserverHandle = FAIMessageObserver::Create(BrainComponent, AGOAPController::DamageMsg, FAIRequestID(3), AIMessageDelegate);
	AGOAPCharacterBase* GOAPPawn = Cast<AGOAPCharacterBase>(InPawn);
	if (!GOAPPawn)
		return;
	GOAPPawn->RegisterGoals(Goals);
	GOAPPawn->RegisterActions(ActionSet);
	AStarComponent->CreateLookupTable(ActionSet);
	current_goal = nullptr;
	NextGoal = nullptr;
	ReEvaluateGoals();
	if (HasGoalChanged())
	{
		current_goal = NextGoal;
		ScreenLog(FString::Printf(TEXT("Goal has changed")));
		RePlan();
	}
}

void AGOAPController::OnUnPossess()
{
	Super::OnUnPossess();
	Goals.Reset();
	current_goal = nullptr;
	AStarComponent->ClearLookupTable();
	GOAPActionsComponent->Reset();
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(this);
}

void AGOAPController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void AGOAPController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	//Might want to register Goals as stimuli listeners and just pump Stimuli to them
	/*
	if (Stimulus.WasSuccessfullySensed())
	{
		//Blackboard->SetValueAsObject(FName("Target"), Actor);
		UE_LOG(LogTemp, Warning, TEXT("Target successfully sensed"));
	}
	else
	{
		//Blackboard->SetValueAsObject(FName("Target"), nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Target not successfully sensed"));
	}
	*/
	ReEvaluateGoals();
	if (HasGoalChanged())
	{
		current_goal = NextGoal;
		RePlan();
	}
}

//TODO: use a Heapified TArray as a PQueue
void AGOAPController::ReEvaluateGoals()
{
	NextGoal = nullptr;
	for (auto Goal : Goals)
	{
		if (Goal->IsGoalValid(this))
		{
			//Recalculate prioirity if necessary
			Goal->ReCalcPriority(this);

			//Highest priority goal becomes next goal
			if (NextGoal == nullptr) 
			{
				NextGoal = Goal;
				continue;
			}
			if (Goal->Priority() > NextGoal->Priority()) 
				NextGoal = Goal;
		}
	}
}

bool AGOAPController::HasGoalChanged()
{
	return NextGoal != current_goal;
}

void AGOAPController::RePlan() 
{

	GOAPActionsComponent->AbortPlan();
	if (current_goal == nullptr)
	{
		return;
	}

	current_goal->Activate(this);

	//No goal -> No plan needed. By default, it'll play the idle animation
	/*
	for (auto Action : ActionSet)
	{
		GOAPActionsComponent->QueueAction(Action);
	}
	*/
	
	TArray<UGOAPAction*> Plan;
	bool bSuccess = AStarComponent->Search(current_goal, current_state, Plan);
	if (bSuccess)
	{
		//ScreenLog(FString::Printf(TEXT("Found Plan")));
		UE_LOG(LogTemp, Warning, TEXT("Plan of size %d"), Plan.Num());
		for (auto Action : Plan)
		{
			//ScreenLog(FString::Printf(TEXT("Queueing action %s"), *Action->GetName()));

			GOAPActionsComponent->QueueAction(Action);
		}

	}
	
	GOAPActionsComponent->RunNextAction();
}

bool AGOAPController::IsPlayingMontage()
{
	ACharacter* GOAPCharacter = Cast<ACharacter>(GetPawn());
	if (!GOAPCharacter)
	{
		return false;
	}

	UAnimInstance* Anim = GOAPCharacter->GetMesh()->GetAnimInstance();
	if (Anim) 
	{
		return Anim->IsAnyMontagePlaying();
	}
	return false;
}

void AGOAPController::OnDamageReceived(UBrainComponent* BrainComp, const FAIMessage& Message)
{
	ScreenLog(FString::Printf(TEXT("Damage received message")));

}
void AGOAPController::OnPlanCompleted()
{
	ScreenLog(FString::Printf(TEXT("Plan Completed")));
}