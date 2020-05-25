#include "..\Public\AIAct_Attack.h"
#include "..\Public\WeaponBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\Public\WorldProperty.h"
#include "..\Public\AITask_AnimMontage.h"

UAIAct_Attack::UAIAct_Attack() :
	Super(
		{ /*FWorldProperty(EWorldKey::kWeaponLoaded, true)*/ },
		{ FWorldProperty(EWorldKey::kTargetDead, true) },
		5
	)
{
}

bool UAIAct_Attack::VerifyContext()
{

	if (!AIOwner)
	{
		return false;
	}
	APawn* Pawn = AIOwner->GetPawn();

	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	if (!BBComp)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid blackboard"));

		return false;
	}
	AWeaponBase* Weapon = Cast<AWeaponBase>(BBComp->GetValueAsObject(FName("EquippedWeapon")));
	if (!Weapon)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid weapon"));
		return false;
	}
	//Would also make sure we have ammo right here

	UAnimMontage* MontageHandle = Weapon->GetFireMontage();
	if (!MontageHandle)
	{
		UE_LOG(LogAction, Error, TEXT("invalid montage"));
		return false;
	}
	CachedMontage = MontageHandle;
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

	MontageTaskHandle = UAITask_AnimMontage::AIAnimMontage(AIOwner, CachedMontage, 1.0f, 3);
	if (!MontageTaskHandle)
	{
		return EActionStatus::kFailed;
	}
	MontageTaskHandle->OnMontageTaskEnded.AddUObject(this, &UAIAct_Attack::StopAction);
	MontageTaskHandle->Activate();
	return EActionStatus::kRunning;
}

void UAIAct_Attack::AbortAction()
{
	Super::AbortAction();
	if (MontageTaskHandle)
	{
		MontageTaskHandle->OnMontageTaskEnded.RemoveAll(this);
		MontageTaskHandle->ExternalCancel();
	}
	MontageTaskHandle = nullptr;
	CachedMontage = nullptr;
}

void UAIAct_Attack::StopAction()
{
	MontageTaskHandle = nullptr;
	CachedMontage = nullptr;
	Super::StopAction();
}