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
	APawn* Pawn = Controller->GetPawn();
	//would check ammo here
	//Need to handle failure/Unbind delegates etc
	return true;
}

EActionStatus UAIAct_Attack::StartAction()
{
	if (Super::StartAction() == EActionStatus::kFailed)
	{
		UE_LOG(LogAction, Error, TEXT("SUPER failed"));
		return EActionStatus::kFailed;
	}

	//TODO: These are so common that they should just be cached
	
	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	if (!BBComp)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid blackboard"));

		return EActionStatus::kFailed;
	}
	ACharacter* ControlledPawn = Cast<ACharacter>(AIOwner->GetPawn());
	if (!ControlledPawn)
	{
		return EActionStatus::kFailed;
	}

	AWeaponBase* Weapon = Cast<AWeaponBase>(BBComp->GetValueAsObject(FName("EquippedWeapon")));
	if (!Weapon)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid weapon"));
		return EActionStatus::kFailed;
	}
	
	//Should go in verify context
	UAnimMontage* MontageHandle = Weapon->GetFireMontage();
	if (!MontageHandle)
	{
		UE_LOG(LogAction, Error, TEXT("invalid montage"));
		return EActionStatus::kFailed;
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
	StopAction();
}

void UAIAct_Attack::OnMontageLoop()
{
}

void UAIAct_Attack::AbortAction()
{
	Super::AbortAction();
	if (TaskHandle)
	{
		TaskHandle->ExternalCancel();
	}
	TaskHandle = nullptr;
}

void UAIAct_Attack::StopAction()
{
	Super::StopAction();
}