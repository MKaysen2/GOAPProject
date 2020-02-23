#include "..\Public\GOAPController.h"


AGOAPController::AGOAPController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	AStarComponent = CreateDefaultSubobject<UAStarComponent>(TEXT("AStarComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	PerceptionComponent->ConfigureSense(*sightConfig);
	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGOAPController::TargetPerceptionUpdated);

	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	UBlackboardData* BBAsset = NewObject<UBlackboardData>(Blackboard);
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Object>(FName("Target"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("FaceTracking"));
	BBAsset->UpdatePersistentKey<UBlackboardKeyType_Bool>(FName("TorsoTracking"));

	Blackboard->InitializeBlackboard(*BBAsset);

	current_state = CreateDefaultSubobject<UWorldState>(TEXT("ControllerState"));
	current_state->add_property(FWorldProperty(EWorldKey::kWeaponLoaded, false));
	current_state->add_property(FWorldProperty(EWorldKey::kHasWeapon, false));
	current_state->add_property(FWorldProperty(EWorldKey::kIdle, false));
	current_state->add_property(FWorldProperty(EWorldKey::kAtLocation, nullptr));
	current_state->add_property(FWorldProperty(EWorldKey::kTargetDead, false));

	//ActionSet.Add(NewObject<UAIAct_MoveTo>());
	//ActionSet.Add(NewObject<UAIAct_Equip>());
	//ActionSet.Add(NewObject<UAIAct_Reload>());
	ActionSet.Add(NewObject<UAIAct_Attack>());
	AnimState = EAnimState::Move;
	current_goal = nullptr;
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
	PerceptionComponent->ConfigureSense(*sightConfig);

	AGOAPCharacterBase* GOAPPawn = Cast<AGOAPCharacterBase>(InPawn);
	if (!GOAPPawn)
		return;
	GOAPPawn->RegisterGoals(Goals);
	AStarComponent->CreateLookupTable(ActionSet);
	current_goal = nullptr;
	NextGoal = nullptr;
	
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

	bool bMoveFinished = (AnimState == EAnimState::Move) && !IsFollowingAPath();
	bool bAnimFinished = (AnimState == EAnimState::Anim) && !IsPlayingMontage();
	//state transition conditions
	if (bMoveFinished || bAnimFinished)
	{
		GOAPActionsComponent->RunNextAction();
	}
	ReEvaluateGoals();
	if (HasGoalChanged())
	{
		current_goal = NextGoal;
		ScreenLog(FString::Printf(TEXT("Goal has changed")));
		RePlan();
	}
}

void AGOAPController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	//Might want to register Goals as stimuli listeners and just pump Stimuli to them
	//
	if (Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsObject(FName("Target"), Actor);
		UE_LOG(LogTemp, Warning, TEXT("Target successfully sensed"));
	}
	else
	{
		Blackboard->SetValueAsObject(FName("Target"), nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Target not successfully sensed"));
	}
}

void AGOAPController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	UE_LOG(LogTemp, Warning, TEXT("Do we actually need to set delegates or does this get called anyway"));
}

//TODO: use a Heapified TArray as a PQueue
void AGOAPController::ReEvaluateGoals()
{
	NextGoal = nullptr;
	for (auto Goal : Goals)
	{
		if (Goal->IsValid(this))
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

void AGOAPController::SetMovementObservers()
{

}

void AGOAPController::SetMontageObservers()
{
	ACharacter* Avatar = Cast<ACharacter>(GetPawn());
	if (!Avatar)
	{
		return;
	}
	UAnimInstance* AnimInstance = Avatar->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	FOnMontageEnded BlendingOutDel;
	BlendingOutDel.BindUObject(this, &AGOAPController::OnMontageBlendingOut);

	FOnMontageEnded MontageEndDel;
	MontageEndDel.BindUObject(this, &AGOAPController::OnMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDel);
	AnimInstance->Montage_SetEndDelegate(MontageEndDel);
}

void AGOAPController::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("Montage Blending out!"));
}

void AGOAPController::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("Montage Ended!"));
}

void AGOAPController::RePlan() 
{

	GOAPActionsComponent->AbortPlan();
	if (IsFollowingAPath())
	{
		StopMovement();
	}
	if (current_goal == nullptr)
	{
		return;
	}
	//No goal -> No plan needed. By default, it'll play the idle animation
	for (auto Action : ActionSet)
	{
		GOAPActionsComponent->QueueAction(Action);
	}
	/*
	TArray<UGOAPAction*> Plan;
	bool bSuccess = AStarComponent->Search(current_goal, current_state, Plan);
	if (bSuccess)
	{
		for (auto Action : Plan)
		{
			GOAPActionsComponent->QueueAction(Action);
		}
	}
	*/
}

bool AGOAPController::IsInState(EAnimState CheckState)
{
	return CheckState == AnimState;
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

void AGOAPController::SetAnimState(const EAnimState& NewState)
{
	AnimState = NewState;
	if (NewState == EAnimState::Anim)
	{
		SetMontageObservers();
	}
	else if (NewState == EAnimState::Move)
	{

	}
}