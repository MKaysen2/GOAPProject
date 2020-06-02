#include "..\Public\AIAct_Attack.h"
#include "..\Public\WeaponBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\Public\WorldProperty.h"
#include "..\Public\AITask_AnimMontage.h"
#include "..\Public\MontageMapComponent.h"

UAIAct_Attack::UAIAct_Attack() :
	Super(5)
{
}

void UAIAct_Attack::InitEffects()
{
	Effects.Reserve(1);
	AddEffect(EWorldKey::kTargetDead, FAISymEffect(EWorldKey::kTargetDead, true));
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