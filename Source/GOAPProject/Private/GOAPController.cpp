#include "../Public/GOAPController.h"
#include "../Public/GOAPCharacterBase.h"
#include "../Public/GOAPActionsComponent.h"
#include "../Public/AStarComponent.h"
#include "../Public/GOAPAction.h"
#include "../Public/GOAPGoal.h"
#include "../Public/WorldState.h"
#include "../Public/StateNode.h"
#include "../Public/GoalSelectionComponent.h"
#include "Perception\AISenseConfig_Sight.h"
#include "Perception\AISenseConfig_Hearing.h"
#include "../Public/InteractableObjectInterface.h"
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
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("CombatTarget"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FName("TargetLocation"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("FaceTracking"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("TorsoTracking"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("EquippedWeapon"));
	Blackboard->InitializeBlackboard(*BBAsset);

	BrainComponent = CreateDefaultSubobject<UBrainComponent>(TEXT("BrainComponent"));

	//TODO: make sure controllers aren't using the same world state.
	//This should probably be a unique ptr
	CurrentState = MakeShared<FWorldState>();

	GoalComponent = CreateDefaultSubobject<UGoalSelectionComponent>(TEXT("GoalComp"));

	GOAPActionsComponent = CreateDefaultSubobject<UGOAPActionsComponent>(TEXT("GOAPActionsComp"));


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

	GoalComponent->OnGoalChanged.BindUObject(this, &AGOAPController::OnGoalChanged);
	GOAPActionsComponent->OnPlanCompleted.BindUObject(this, &AGOAPController::OnPlanCompleted);

	TestObserverHandle = FAIMessageObserver::Create(BrainComponent, AGOAPController::DamageMsg, FAIRequestID(3), AIMessageDelegate);
	AGOAPCharacterBase* GOAPPawn = Cast<AGOAPCharacterBase>(InPawn);
	if (!GOAPPawn)
		return;
	GoalComponent->RegisterGoalSet(GOAPPawn->GetGoalSet());
	GOAPActionsComponent->RegisterActionSet(GOAPPawn->GetActionSet());
	AStarComponent->CreateLookupTable(GOAPActionsComponent->GetActionSet());
	//GoalComponent->ReEvaluateGoals();

}

void AGOAPController::OnUnPossess()
{
	Super::OnUnPossess();
	AStarComponent->ClearLookupTable();
	GoalComponent->Reset();
	GOAPActionsComponent->Reset();
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(this);
}

void AGOAPController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);	
}

void AGOAPController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	//GoalComponent->ReEvaluateGoals();
}

void AGOAPController::RePlan() 
{
	//TODO: limit number of RePlans per tick to 
	UGOAPGoal* CurrentGoal = GoalComponent->GetCurrentGoal();
	GOAPActionsComponent->AbortPlan();
	if (CurrentGoal == nullptr)
	{
		return;
	}
	//No goal -> No plan needed. By default, it'll play the idle animation
	//TODO: if the plan fails, replan for the next highest goal
	//TSharedPtr<FStateNode> Node = AStarComponent->Search(CurrentGoal, CurrentState);
	/*
	if (Node.IsValid())
	{
		//GOAPActionsComponent->StartPlan(Node);
	}
	else
	{
		GoalComponent->OnGoalCompleted();
	}
	*/
	
}

void AGOAPController::ApplyWorldProp(const EWorldKey kWorldKey, const bool bValue)
{
	
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

void AGOAPController::OnGoalChanged(UGOAPGoal* NewGoal)
{
	RePlan();
}

void AGOAPController::OnDamageReceived(UBrainComponent* BrainComp, const FAIMessage& Message)
{
	ScreenLog(FString::Printf(TEXT("Damage received message")));

}

void AGOAPController::OnPlanCompleted()
{
	//ScreenLog(FString::Printf(TEXT("Plan Completed")));
	GoalComponent->OnGoalCompleted();
}