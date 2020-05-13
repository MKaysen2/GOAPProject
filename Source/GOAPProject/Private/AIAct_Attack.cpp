#include "..\Public\AIAct_Attack.h"
#include "..\Public\CombatInterface.h"
#include "..\Public\WeaponBase.h"
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
	UAnimMontage* FireMontage = Weapon->GetFireMontage();
	if (!FireMontage)
	{
		UE_LOG(LogAction, Error, TEXT("invalid montage"));
	}
	FOnMontageEnded MontageEndedDel = FOnMontageEnded::CreateUObject(this, &UAIAct_Attack::OnMontageEnded, Controller);

	ControlledPawn->PlayAnimMontage(FireMontage);
	Mesh->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDel);
	return EActionStatus::kRunning;
}

void UAIAct_Attack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, AAIController* Controller)
{
	UE_LOG(LogAction, Warning, TEXT("MontageEnded Callback"));
	StopAction(Controller);
}

void UAIAct_Attack::AbortAction(AAIController* Controller)
{
	Super::AbortAction(Controller);
	
}
