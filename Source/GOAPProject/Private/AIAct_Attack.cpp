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
	/*	
	UAnimMontage* FireMontage = Weapon->GetFireMontage();
	if (!FireMontage)
	{
		UE_LOG(LogAction, Error, TEXT("invalid montage"));
	}
	*/
	UAnimSequenceBase* FireSequence = Weapon->GetFireSequence();
	if (!FireSequence)
	{
		UE_LOG(LogAction, Error, TEXT("Invalid Sequence"));
		return EActionStatus::kFailed;
	}


	MontageHandle = ControlledPawn->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(FireSequence, FName(TEXT("DefaultSlot")));
	float Duration = MontageHandle->GetSectionLength(0);

	Controller->GetWorldTimerManager().SetTimer(MontageTimerHandle, this, &UAIAct_Attack::OnMontageEnded, Duration, false);
	return EActionStatus::kRunning;
}

void UAIAct_Attack::OnMontageEnded()
{
	StopAction(AIOwner);
}

void UAIAct_Attack::AbortAction(AAIController* Controller)
{
	Super::AbortAction(Controller);
	AIOwner->GetWorldTimerManager().ClearTimer(MontageTimerHandle);
	ACharacter* ControlledPawn = Cast<ACharacter>(Controller->GetPawn());
	ControlledPawn->GetMesh()->GetAnimInstance()->Montage_Stop(0.5, MontageHandle);
	MontageHandle = nullptr;
}

void UAIAct_Attack::StopAction(AAIController* Controller)
{
	Super::StopAction(Controller);
	bool bIsTimerActive = AIOwner->GetWorldTimerManager().IsTimerActive(MontageTimerHandle);
	if (bIsTimerActive)
	{
		AIOwner->GetWorldTimerManager().ClearTimer(MontageTimerHandle);
	}
	if (MontageHandle == nullptr)
	{
		return;
	}
	ACharacter* ControlledPawn = Cast<ACharacter>(Controller->GetPawn());
	ControlledPawn->GetMesh()->GetAnimInstance()->Montage_Stop(0.5, MontageHandle);
	MontageHandle = nullptr;
}