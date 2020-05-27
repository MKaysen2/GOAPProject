#include "../Public/AIAct_ThrowGrenade.h"
#include "../Public/AITask_AnimMontage.h"
#include "AIController.h"
#include "../Public/MontageMapComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UAIAct_ThrowGrenade::UAIAct_ThrowGrenade() :
	Super(
		{  },
		{ FWorldProperty(EWorldKey::kTargetDead, true) },
		2
	)
{
}

bool UAIAct_ThrowGrenade::VerifyContext()
{

	if (!AIOwner)
	{
		return false;
	}

	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	if (!BBComp)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid blackboard"));

		return false;
	}
	UMontageMapComponent* MontageMapComp = Cast<UMontageMapComponent>(BBComp->GetValueAsObject(FName("MontageMapComp")));
	if (!MontageMapComp)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid MontageMapper"));
		return false;
	}
	//Would also make sure we have ammo right here

	UAnimMontage* MontageHandle = MontageMapComp->GetMontageByName(FName("Grenade"));
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

EActionStatus UAIAct_ThrowGrenade::StartAction()
{
	if (Super::StartAction() == EActionStatus::kFailed)
	{
		UE_LOG(LogAction, Error, TEXT("SUPER failed"));
		return EActionStatus::kFailed;
	}

	MontageTaskHandle = UAITask_AnimMontage::AIAnimMontage(AIOwner, CachedMontage, 1.0f);
	if (!MontageTaskHandle)
	{
		return EActionStatus::kFailed;
	}
	MontageTaskHandle->OnMontageTaskEnded.AddUObject(this, &UAIAct_ThrowGrenade::StopAction);
	MontageTaskHandle->Activate();
	return EActionStatus::kRunning;
}

void UAIAct_ThrowGrenade::StopAction()
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

void UAIAct_ThrowGrenade::AbortAction()
{
	MontageTaskHandle = nullptr;
	CachedMontage = nullptr;
	Super::StopAction();
}
