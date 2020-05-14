#include "..\Public\AIAct_Attack.h"
#include "..\Public\CombatInterface.h"
#include "..\Public\WeaponBase.h"
#include "TimerManager.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "..\Public\WorldProperty.h"
#include "..\Public\WorldState.h"
#include "..\Public\AITask_AnimMontage.h"
#include "GameFramework/Pawn.h"


UAIAct_Attack::UAIAct_Attack() : 
	Super(
		{ /*FWorldProperty(EWorldKey::kWeaponLoaded, true)*/ },
		{ FWorldProperty(EWorldKey::kTargetDead, true) },
		5
	)
{
}

bool UAIAct_Attack::VerifyContext(AAIController* Controller)
{

	if (!Controller)
	{
		return false;
	}
	AIOwner = Controller;
	APawn* Pawn = Controller->GetPawn();
	//would check ammo here
	//Need to handle failure/Unbind delegates etc
	return true;
}

EActionStatus UAIAct_Attack::StartAction(AAIController* Controller)
{
	if (Super::StartAction(Controller) == EActionStatus::kFailed)
	{
		UE_LOG(LogAction, Error, TEXT("SUPER failed"));
		return EActionStatus::kFailed;
	}

	//TODO: separate all of this to an ai task
	
	UBlackboardComponent* BBComp = Controller->GetBlackboardComponent();
	if (!BBComp)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid blackboard"));

		return EActionStatus::kFailed;
	}
	ACharacter* ControlledPawn = Cast<ACharacter>(Controller->GetPawn());
	if (!ControlledPawn)
	{
		return EActionStatus::kFailed;
	}

	USkeletalMeshComponent* Mesh = ControlledPawn->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid Skelmesh"));
		return EActionStatus::kFailed;
	}
	AWeaponBase* Weapon = Cast<AWeaponBase>(BBComp->GetValueAsObject(FName("EquippedWeapon")));
	if (!Weapon)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid weapon"));
		return EActionStatus::kFailed;
	}
	
	UAnimMontage* MontageHandle = Weapon->GetFireMontage();
	if (!MontageHandle)
	{
		UE_LOG(LogAction, Error, TEXT("invalid montage"));
	}

	UAITask_AnimMontage* MontageTask = UAITask_AnimMontage::AIAnimMontage(AIOwner, MontageHandle, 1.0f, 5);
	if (!MontageTask)
	{
		return EActionStatus::kFailed;
	}
	TaskHandle = MontageTask;
	MontageTask->OnMontageTaskEnded.AddUObject(this, &UAIAct_Attack::OnMontageEnded);
	MontageTask->Activate();
	return EActionStatus::kRunning;
}

void UAIAct_Attack::OnMontageEnded()
{
	UE_LOG(LogAction, Warning, TEXT("ActionEnded"));
	StopAction(AIOwner);
}

void UAIAct_Attack::OnMontageLoop()
{
}

void UAIAct_Attack::AbortAction(AAIController* Controller)
{
	Super::AbortAction(Controller);
}

void UAIAct_Attack::StopAction(AAIController* Controller)
{
	Super::StopAction(Controller);
}